from app import app, db, Level, User
import os
import shutil

with app.app_context():
    levels = Level.query.all()
    
    for level in levels:
        if level.slot >= 0:
            # Новое имя файла по слоту
            user = User.query.get(level.user_id)
            save_dir = os.path.join(os.getcwd(), "levels", user.username)
            new_file_path = os.path.join(save_dir, f"{level.slot}.dat.gz")
            
            # Если старый файл существует и отличается от нового
            if os.path.exists(level.file_path) and level.file_path != new_file_path:
                print(f"Moving: {level.file_path} -> {new_file_path}")
                shutil.move(level.file_path, new_file_path)
                level.file_path = new_file_path
            elif not os.path.exists(level.file_path):
                print(f"WARNING: File missing for slot {level.slot}: {level.file_path}")
                # Устанавливаем правильный путь даже если файл отсутствует
                level.file_path = new_file_path
    
    db.session.commit()
    print("Migration complete!")
