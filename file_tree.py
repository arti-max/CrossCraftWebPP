#!/usr/bin/env python3
"""
Скрипт для вывода дерева файлов проекта с подсчётом строк кода
Использование: python tree_project.py [путь] [--exclude dir1,dir2]
"""

import os
import sys
from pathlib import Path

# Файлы и папки которые нужно игнорировать
DEFAULT_IGNORE = {
    '.git', '.vscode', '.idea', 'node_modules', 'build', 'dist',
    '__pycache__', '.cache', '.DS_Store', 'CMakeFiles',
    '.gitignore', '.gitmodules', 'cmake_install.cmake',
    'CMakeCache.txt', 'Makefile', '*.o', '*.a', '*.so', '*.exe'
}

# Расширения текстовых файлов для подсчёта строк
CODE_EXTENSIONS = {
    '.cpp', '.hpp', '.h', '.c', '.cc', '.cxx',
    '.py', '.js', '.ts', '.tsx', '.jsx',
    '.java', '.cs', '.go', '.rs', '.swift',
    '.php', '.rb', '.pl', '.sh', '.bash',
    '.html', '.css', '.scss', '.sass',
    '.json', '.xml', '.yaml', '.yml',
    '.md', '.txt', '.cmake', '.makefile'
}

def should_ignore(path, ignore_patterns):
    """Проверяет нужно ли игнорировать путь"""
    name = path.name
    
    # Проверка точных совпадений
    if name in ignore_patterns:
        return True
    
    # Проверка масок (например *.o)
    for pattern in ignore_patterns:
        if '*' in pattern:
            if pattern.startswith('*') and name.endswith(pattern[1:]):
                return True
            if pattern.endswith('*') and name.startswith(pattern[:-1]):
                return True
    
    return False

def count_lines(file_path):
    """
    Подсчитывает количество строк в файле
    Возвращает (total_lines, code_lines, empty_lines, comment_lines)
    """
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
            
        total = len(lines)
        empty = sum(1 for line in lines if line.strip() == '')
        
        # Простой подсчёт комментариев (не идеален, но достаточно)
        comments = sum(1 for line in lines 
                      if line.strip().startswith('//') or 
                         line.strip().startswith('#') or
                         line.strip().startswith('/*') or
                         line.strip().startswith('*'))
        
        code = total - empty - comments
        
        return total, code, empty, comments
    except:
        return 0, 0, 0, 0

def is_code_file(file_path):
    """Проверяет является ли файл кодом"""
    return file_path.suffix.lower() in CODE_EXTENSIONS

def format_size(size):
    """Форматирует размер файла в человекочитаемый вид"""
    for unit in ['B', 'KB', 'MB', 'GB']:
        if size < 1024.0:
            return f"{size:.1f}{unit}"
        size /= 1024.0
    return f"{size:.1f}TB"

def get_tree(path, prefix="", ignore_patterns=None, max_depth=None, current_depth=0, stats=None):
    """
    Рекурсивно строит дерево файлов с подсчётом строк
    
    Args:
        path: путь к директории
        prefix: префикс для отступов
        ignore_patterns: набор паттернов для игнорирования
        max_depth: максимальная глубина
        current_depth: текущая глубина
        stats: словарь для накопления статистики
    """
    if ignore_patterns is None:
        ignore_patterns = DEFAULT_IGNORE
    
    if stats is None:
        stats = {
            'total_files': 0,
            'code_files': 0,
            'total_lines': 0,
            'code_lines': 0,
            'empty_lines': 0,
            'comment_lines': 0,
            'total_size': 0
        }
    
    if max_depth is not None and current_depth >= max_depth:
        return [], stats
    
    lines = []
    path = Path(path)
    
    try:
        # Получаем список файлов и папок
        items = sorted(path.iterdir(), key=lambda x: (not x.is_dir(), x.name.lower()))
        items = [item for item in items if not should_ignore(item, ignore_patterns)]
        
        for i, item in enumerate(items):
            is_last = (i == len(items) - 1)
            
            # Символы дерева
            connector = "└── " if is_last else "├── "
            extension = "    " if is_last else "│   "
            
            if item.is_dir():
                lines.append(f"{prefix}{connector}{item.name}/")
                # Рекурсивно обходим папку
                sublines, stats = get_tree(
                    item, 
                    prefix + extension, 
                    ignore_patterns,
                    max_depth,
                    current_depth + 1,
                    stats
                )
                lines.extend(sublines)
            else:
                stats['total_files'] += 1
                size = item.stat().st_size
                stats['total_size'] += size
                size_str = format_size(size)
                
                # Если это файл с кодом, подсчитываем строки
                if is_code_file(item):
                    stats['code_files'] += 1
                    total, code, empty, comments = count_lines(item)
                    stats['total_lines'] += total
                    stats['code_lines'] += code
                    stats['empty_lines'] += empty
                    stats['comment_lines'] += comments
                    
                    lines.append(f"{prefix}{connector}{item.name} ({size_str}, {total} lines)")
                else:
                    lines.append(f"{prefix}{connector}{item.name} ({size_str})")
    
    except PermissionError:
        lines.append(f"{prefix}[Permission Denied]")
    
    return lines, stats

def print_tree(path, ignore=None, max_depth=None, show_stats=True):
    """
    Выводит дерево файлов с подсчётом строк
    """
    path = Path(path)
    
    if not path.exists():
        print(f"Ошибка: путь {path} не существует")
        return
    
    # Объединяем паттерны игнорирования
    ignore_patterns = DEFAULT_IGNORE.copy()
    if ignore:
        ignore_patterns.update(ignore)
    
    # Выводим корневую директорию
    print(f"\n📁 {path.absolute()}")
    print("─" * 80)
    
    # Строим дерево
    stats = {
        'total_files': 0,
        'code_files': 0,
        'total_lines': 0,
        'code_lines': 0,
        'empty_lines': 0,
        'comment_lines': 0,
        'total_size': 0
    }
    
    tree_lines, stats = get_tree(path, "", ignore_patterns, max_depth, 0, stats)
    
    # Выводим дерево
    for line in tree_lines:
        print(line)
    
    # Выводим статистику
    if show_stats:
        print("─" * 80)
        print(f"📊 Статистика проекта:")
        print(f"   • Всего файлов: {stats['total_files']}")
        print(f"   • Файлов с кодом: {stats['code_files']}")
        print(f"   • Общий размер: {format_size(stats['total_size'])}")
        print()
        print(f"📝 Подсчёт строк кода:")
        print(f"   • Всего строк: {stats['total_lines']:,}")
        print(f"   • Строк кода: {stats['code_lines']:,}")
        print(f"   • Пустых строк: {stats['empty_lines']:,}")
        print(f"   • Комментариев: {stats['comment_lines']:,}")
        
        if stats['total_lines'] > 0:
            code_percent = (stats['code_lines'] / stats['total_lines']) * 100
            empty_percent = (stats['empty_lines'] / stats['total_lines']) * 100
            comment_percent = (stats['comment_lines'] / stats['total_lines']) * 100
            print()
            print(f"📈 Процентное соотношение:")
            print(f"   • Код: {code_percent:.1f}%")
            print(f"   • Пустые строки: {empty_percent:.1f}%")
            print(f"   • Комментарии: {comment_percent:.1f}%")

def main():
    """Главная функция"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Выводит дерево файлов проекта с подсчётом строк кода',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Примеры использования:
  python tree_project.py                    # Текущая директория
  python tree_project.py /path/to/project   # Указанная директория
  python tree_project.py --depth 2          # Максимум 2 уровня вложенности
  python tree_project.py --exclude "test,tmp"  # Игнорировать test и tmp
  python tree_project.py --no-stats         # Без статистики
  python tree_project.py -o tree.txt        # Сохранить в файл
        """
    )
    
    parser.add_argument(
        'path',
        nargs='?',
        default='.',
        help='Путь к директории (по умолчанию текущая)'
    )
    
    parser.add_argument(
        '--depth', '-d',
        type=int,
        default=None,
        help='Максимальная глубина вложенности'
    )
    
    parser.add_argument(
        '--exclude', '-e',
        type=str,
        default='',
        help='Дополнительные папки для игнорирования (через запятую)'
    )
    
    parser.add_argument(
        '--no-stats',
        action='store_true',
        help='Не показывать статистику'
    )
    
    parser.add_argument(
        '--output', '-o',
        type=str,
        default=None,
        help='Сохранить в файл'
    )
    
    args = parser.parse_args()
    
    # Парсим дополнительные исключения
    extra_ignore = set()
    if args.exclude:
        extra_ignore = set(args.exclude.split(','))
    
    # Перенаправляем вывод в файл если нужно
    if args.output:
        original_stdout = sys.stdout
        sys.stdout = open(args.output, 'w', encoding='utf-8')
    
    try:
        print_tree(
            args.path,
            ignore=extra_ignore,
            max_depth=args.depth,
            show_stats=not args.no_stats
        )
    finally:
        if args.output:
            sys.stdout.close()
            sys.stdout = original_stdout
            print(f"✅ Дерево файлов сохранено в {args.output}")

if __name__ == '__main__':
    main()
