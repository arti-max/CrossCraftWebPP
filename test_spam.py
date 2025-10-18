#!/usr/bin/env python3
"""
Продвинутый тест защиты от спама
Использует более умные техники обхода
"""

import requests
import random
import string
import time
from datetime import datetime

BASE_URL = "http://crosscraftweb.ddns.net"
REGISTER_URL = f"{BASE_URL}/register.jsp"
ATTEMPTS = 100

stats = {
    'total': 0,
    'success': 0,
    'blocked': 0,
    'errors': {}
}

# Реальные имена для обхода детекции
REAL_NAMES = [
    "Alex", "Bob", "Charlie", "David", "Eve", "Frank", "Grace", "Helen",
    "Igor", "Jack", "Kate", "Leo", "Mary", "Nick", "Olivia", "Paul",
    "Quinn", "Rose", "Sam", "Tina", "Uma", "Vince", "Wade", "Xena",
    "Yuri", "Zoe", "Anna", "Ben", "Clara", "Dan", "Emma", "Fred"
]

# Реальные слова для email
REAL_WORDS = [
    "dev", "user", "admin", "test", "player", "gamer", "coder", "ninja",
    "master", "pro", "king", "queen", "star", "hero", "legend", "champion"
]

def generate_smart_username():
    """Генерирует умный username, похожий на настоящий"""
    patterns = [
        # Реальное имя + цифра или две
        lambda: random.choice(REAL_NAMES) + str(random.randint(1, 99)),
        
        # Реальное имя + слово
        lambda: random.choice(REAL_NAMES) + random.choice(REAL_WORDS).capitalize(),
        
        # Слово + цифра
        lambda: random.choice(REAL_WORDS) + str(random.randint(10, 999)),
        
        # Два реальных слова
        lambda: random.choice(REAL_WORDS) + random.choice(REAL_WORDS),
        
        # Реальное имя с подчёркиванием
        lambda: random.choice(REAL_NAMES).lower() + "_" + random.choice(REAL_WORDS),
        
        # CamelCase
        lambda: random.choice(REAL_NAMES) + random.choice(REAL_NAMES),
        
        # Легитимный никнейм с числами (но не в начале)
        lambda: random.choice(REAL_WORDS) + random.choice(['x', 'z', 'r']) + str(random.randint(1, 9)),
    ]
    
    return random.choice(patterns)()

def generate_smart_email():
    """Генерирует умный email, похожий на настоящий"""
    
    # Легитимные домены
    domains = ['gmail.com', 'yahoo.com', 'outlook.com', 'hotmail.com', 'icloud.com']
    
    patterns = [
        # Имя.фамилия@домен
        lambda: random.choice(REAL_NAMES).lower() + '.' + 
                random.choice(REAL_NAMES).lower() + '@' + 
                random.choice(domains),
        
        # имя_слово@домен
        lambda: random.choice(REAL_NAMES).lower() + '_' + 
                random.choice(REAL_WORDS) + '@' + 
                random.choice(domains),
        
        # слово.цифра@домен (не много цифр)
        lambda: random.choice(REAL_WORDS) + '.' + 
                str(random.randint(1, 99)) + '@' + 
                random.choice(domains),
        
        # имяслово@домен
        lambda: random.choice(REAL_NAMES).lower() + 
                random.choice(REAL_WORDS) + '@' + 
                random.choice(domains),
        
        # слово-слово@домен
        lambda: random.choice(REAL_WORDS) + '-' + 
                random.choice(REAL_WORDS) + '@' + 
                random.choice(domains),
    ]
    
    email = random.choice(patterns)()
    
    # Убедимся что email не слишком длинный
    local = email.split('@')[0]
    if len(local) > 12:
        # Укорачиваем
        local = local[:12]
        email = local + '@' + email.split('@')[1]
    
    return email

def generate_legit_password():
    """Генерирует реалистичный пароль"""
    patterns = [
        lambda: random.choice(REAL_WORDS) + str(random.randint(100, 999)),
        lambda: random.choice(REAL_NAMES) + str(random.randint(1000, 9999)),
        lambda: ''.join(random.choices(string.ascii_letters + string.digits, k=random.randint(8, 12))),
    ]
    return random.choice(patterns)()

def attempt_registration(username, email, password):
    """Пытается зарегистрировать аккаунт"""
    data = {
        'name': username,
        'email': email,
        'password': password,
        'password_again': password
    }
    
    try:
        response = requests.post(REGISTER_URL, data=data, allow_redirects=False, timeout=5)
        
        if response.status_code == 302:
            location = response.headers.get('Location', '')
            if 'login' in location:
                return 'success', 'Registered successfully ⚠️'
            else:
                return 'blocked', 'Redirected elsewhere'
        
        elif response.status_code == 200:
            text = response.text.lower()
            if 'suspicious' in text:
                return 'blocked', 'Detected as suspicious'
            elif 'temporary email' in text:
                return 'blocked', 'Temporary email blocked'
            elif 'too many' in text:
                return 'blocked', 'Rate limited'
            else:
                return 'blocked', 'Unknown block (200)'
        
        elif response.status_code == 400:
            return 'blocked', 'HTTP 400 (validation error)'
        
        elif response.status_code == 403:
            return 'blocked', 'HTTP 403 (forbidden)'
        
        elif response.status_code == 429:
            return 'blocked', 'HTTP 429 (rate limited)'
        
        else:
            return 'error', f'HTTP {response.status_code}'
    
    except requests.exceptions.RequestException as e:
        return 'error', f'Network error: {str(e)[:30]}'

def print_progress_bar(current, total, status):
    bar_length = 40
    filled = int(bar_length * current / total)
    bar = '█' * filled + '░' * (bar_length - filled)
    percent = current / total * 100
    print(f'\r[{bar}] {percent:.1f}% | {current}/{total} | {status}', end='', flush=True)

def main():
    print("=" * 70)
    print("🧠 CROSSCRAFT ADVANCED SPAM PROTECTION TEST")
    print("=" * 70)
    print(f"Target:   {BASE_URL}")
    print(f"Attempts: {ATTEMPTS}")
    print(f"Strategy: Smart username/email generation with legitimate patterns")
    print("=" * 70)
    print()
    
    start_time = time.time()
    successful_accounts = []
    
    for i in range(1, ATTEMPTS + 1):
        username = generate_smart_username()
        email = generate_smart_email()
        password = generate_legit_password()
        
        stats['total'] += 1
        
        result, reason = attempt_registration(username, email, password)
        
        if result == 'success':
            stats['success'] += 1
            status_emoji = '⚠️ '
            successful_accounts.append((username, email))
        elif result == 'blocked':
            stats['blocked'] += 1
            status_emoji = '🚫'
            stats['errors'][reason] = stats['errors'].get(reason, 0) + 1
        else:
            status_emoji = '❌'
            stats['errors'][reason] = stats['errors'].get(reason, 0) + 1
        
        status_text = f"{status_emoji} {username[:18]:<18} | {email[:30]:<30}"
        print_progress_bar(i, ATTEMPTS, status_text)
        
        # Небольшая задержка + случайная задержка для обхода rate limit
        time.sleep(random.uniform(0.2, 0.5))
    
    elapsed_time = time.time() - start_time
    
    print("\n")
    print("=" * 70)
    print("📊 RESULTS")
    print("=" * 70)
    print(f"Total attempts:     {stats['total']}")
    print(f"✅ Successful:       {stats['success']} ({stats['success']/stats['total']*100:.1f}%)")
    print(f"🚫 Blocked:          {stats['blocked']} ({stats['blocked']/stats['total']*100:.1f}%)")
    print(f"⏱️  Time elapsed:     {elapsed_time:.2f}s")
    print(f"⚡ Rate:             {stats['total']/elapsed_time:.2f} req/s")
    print()
    
    if successful_accounts:
        print("=" * 70)
        print("⚠️  SUCCESSFUL SPAM ACCOUNTS (SECURITY ISSUE!)")
        print("=" * 70)
        for username, email in successful_accounts:
            print(f"  {username:<20} | {email}")
        print()
    
    if stats['errors']:
        print("=" * 70)
        print("🔍 BLOCK REASONS")
        print("=" * 70)
        for reason, count in sorted(stats['errors'].items(), key=lambda x: x[1], reverse=True):
            print(f"  {count:3d}x  {reason}")
        print()
    
    print("=" * 70)
    print("🎯 VERDICT")
    print("=" * 70)
    
    success_rate = stats['success'] / stats['total'] * 100
    
    if success_rate == 0:
        print("✅ EXCELLENT: All advanced spam attempts were blocked!")
        print("   Protection is working perfectly against smart attackers.")
    elif success_rate < 5:
        print("✅ GOOD: Protection is strong (< 5% bypass rate)")
        print("   Minor improvements recommended.")
    elif success_rate < 10:
        print("⚠️  WARNING: Some smart spam got through (< 10% bypass)")
        print("   Consider strengthening validation rules.")
    elif success_rate < 20:
        print("⚠️  CRITICAL: Significant bypass rate (< 20%)")
        print("   Protection needs improvement!")
    else:
        print("❌ CRITICAL: Protection is weak against smart attackers!")
        print("   Immediate action required!")
    
    print("=" * 70)

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n⚠️  Test interrupted by user")
    except Exception as e:
        print(f"\n\n❌ Error: {e}")
