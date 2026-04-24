# app.py

import os
import secrets
from flask import Flask, render_template, request, redirect, url_for, send_from_directory, flash, session, abort, send_file
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager, UserMixin, login_user, logout_user, login_required, current_user
from werkzeug.security import generate_password_hash, check_password_hash
from werkzeug.middleware.proxy_fix import ProxyFix
from werkzeug.utils import secure_filename
from flask_cors import CORS
import logging
from logging.handlers import RotatingFileHandler
import re
import time
from datetime import datetime, timedelta
from flask_mail import Mail, Message
from itsdangerous import URLSafeTimedSerializer
import threading
from flask_migrate import Migrate
from urllib.parse import unquote

REGISTRATION_ATTEMPTS = {}
MAX_REGISTRATIONS_PER_IP = 10
REGISTRATION_COOLDOWN = 3600
SKIN_FOLDER = os.path.join(os.getcwd(), 'skins')
ALLOWED_EXTENSIONS = {'png', 'jpg', 'jpeg'}

app = Flask(__name__)
CORS(app)
app.wsgi_app = ProxyFix(app.wsgi_app, x_for=1, x_proto=1, x_host=1, x_prefix=1)

app.config['SECRET_KEY'] = 'cross13craft78web2'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///users.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['MAIL_SERVER'] = 'smtp.gmail.com'
app.config['MAIL_PORT'] = 587
app.config['MAIL_USE_TLS'] = True
app.config['MAIL_USERNAME'] = 'ktemyc765@gmail.com'
app.config['MAIL_PASSWORD'] = 'ucuacsnascfqqrmu'
app.config['MAIL_DEFAULT_SENDER'] = 'CrossCraft <ktemyc765@gmail.com>'
app.config['SKIN_FOLDER'] = SKIN_FOLDER
app.config['MAX_CONTENT_LENGTH'] = 2 * 1024 * 1024
app.config['APRIL_FOOLS_ENABLED'] = False

if not os.path.exists(SKIN_FOLDER):
    os.makedirs(SKIN_FOLDER)


mail = Mail(app)
db = SQLAlchemy(app)
migrate = Migrate(app, db)
serializer = URLSafeTimedSerializer(app.config['SECRET_KEY'])
login_manager = LoginManager(app)
login_manager.login_view = 'login'

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

def generate_confirmation_token(email):
    return serializer.dumps(email, salt='email-confirm-salt')

def confirm_token(token, expiration=3600):
    try:
        email = serializer.loads(token, salt='email-confirm-salt', max_age=expiration)
        return email
    except:
        return None

def send_confirmation_email(user_email, username):
    token = generate_confirmation_token(user_email)
    confirm_url = url_for('confirm_email', token=token, _external=True)
    
    html = f"""
    <h2>Welcome to CrossCraft, {username}!</h2>
    <p>Thank you for registering. Please confirm your email address by clicking the link below:</p>
    <p><a href="{confirm_url}">Confirm Email</a></p>
    <p>This link will expire in 1 hour.</p>
    <p>If you didn't register, please ignore this email.</p>
    <hr>
    <p><small>CrossCraft Web - Minecraft Classic in your browser</small></p>
    """
    
    msg = Message('Confirm your email - CrossCraft', recipients=[user_email])
    msg.html = html
    
    try:
        mail.send(msg)
        return True
    except Exception as e:
        app.logger.error(f"Failed to send email: {e}")
        return False
    
class Server(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(100), nullable=False)
    users = db.Column(db.Integer, nullable=False)
    max_users = db.Column(db.Integer, nullable=False)
    ip_address = db.Column(db.String(45), nullable=False)
    port = db.Column(db.Integer, nullable=False)
    is_public = db.Column(db.Boolean, default=False, nullable=False)
    last_heartbeat = db.Column(db.DateTime, default=datetime.utcnow, onupdate=datetime.utcnow)

    __table_args__ = (db.UniqueConstraint('ip_address', 'port', name='_ip_port_uc'),)

class User(UserMixin, db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(80), unique=True, nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    password_hash = db.Column(db.String(120), nullable=False)
    email_confirmed = db.Column(db.Boolean, default=False)
    confirmed_at = db.Column(db.DateTime, nullable=True)
    has_license = db.Column(db.Boolean, default=False, nullable=False)
    created_at = db.Column(db.DateTime, default=datetime.utcnow)
    current_session_id = db.Column(db.String(100), nullable=True) 
    
    def set_password(self, password):
        self.password_hash = generate_password_hash(password)
    
    def check_password(self, password):
        return check_password_hash(self.password_hash, password)

class Level(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    slot = db.Column(db.Integer, nullable=False, index=True)
    name = db.Column(db.String(100), nullable=False)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'), nullable=False)
    file_path = db.Column(db.String(200), nullable=False, unique=True)
    creation_time = db.Column(db.DateTime, server_default=db.func.now())

@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))

# ✅ НОВАЯ ФУНКЦИЯ: Удаление неподтверждённых аккаунтов старше 1 часа
def cleanup_unconfirmed_users():
    """Удаляет неподтверждённые аккаунты старше 1 часа"""
    with app.app_context():
        try:
            cutoff_time = datetime.utcnow() - timedelta(hours=1)
            
            # Находим неподтверждённые аккаунты старше 1 часа
            expired_users = User.query.filter(
                User.email_confirmed == False,
                User.created_at < cutoff_time
            ).all()
            
            if expired_users:
                app.logger.info(f"Cleaning up {len(expired_users)} unconfirmed accounts")
                
                for user in expired_users:
                    # Удаляем связанные уровни (если есть)
                    levels = Level.query.filter_by(user_id=user.id).all()
                    for level in levels:
                        try:
                            if os.path.exists(level.file_path):
                                os.remove(level.file_path)
                        except:
                            pass
                        db.session.delete(level)
                    
                    app.logger.info(f"Deleting unconfirmed user: {user.username} ({user.email})")
                    db.session.delete(user)
                
                db.session.commit()
                app.logger.info("Cleanup completed")
        
        except Exception as e:
            app.logger.error(f"Cleanup error: {e}")
            db.session.rollback()

# ✅ ФОНОВАЯ ЗАДАЧА: Запуск очистки каждые 15 минут
def start_cleanup_scheduler():
    """Запускает периодическую очистку неподтверждённых аккаунтов"""
    def run_cleanup():
        while True:
            time.sleep(900)  # 15 минут
            cleanup_unconfirmed_users()
    
    thread = threading.Thread(target=run_cleanup, daemon=True)
    thread.start()
    app.logger.info("Cleanup scheduler started (runs every 15 minutes)")

TEMPLATES_DIR = 'templates'
GAME_DIR = os.path.join(TEMPLATES_DIR, 'game')
DEV_DIR = os.path.join(TEMPLATES_DIR, 'dev')
RESOURCES_DIR = os.path.join(os.getcwd(), 'resources')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/register.jsp', methods=['GET', 'POST'])
def register():
    if current_user.is_authenticated:
        return redirect(url_for('index'))
    
    if request.method == 'POST':
        username = request.form.get('name', '').strip()
        email = request.form.get('email', '').strip()
        password = request.form.get('password', '')
        password_again = request.form.get('password_again', '')
        
        # Email формат
        email_regex = r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$'
        if not re.match(email_regex, email):
            return render_template('register.html', error='Invalid email format'), 400
        
        # Блокировка спам-доменов
        spam_domains = ['foxroids.com', 'tempmail.net', '10minutemail.com']
        if any(domain in email.lower() for domain in spam_domains):
            return render_template('register.html', error='Temporary email not allowed'), 403
        
        # Username валидация
        if not 3 <= len(username) <= 20:
            return render_template('register.html', error='Username: 3-20 characters'), 400
        
        if not re.match(r'^[a-zA-Z0-9_-]+$', username):
            return render_template('register.html', error='Username: letters, numbers, - and _ only'), 400
        
        # Пароли
        if len(password) < 6:
            return render_template('register.html', error='Password: minimum 6 characters'), 400
        
        if password != password_again:
            return render_template('register.html', error='Passwords do not match'), 400
        
        # Проверка существования
        if User.query.filter_by(username=username).first():
            return render_template('register.html', error='Username already exists'), 400
        
        if User.query.filter_by(email=email).first():
            return render_template('register.html', error='Email already registered'), 400
        
        # Rate limiting
        import json
        rate_limit_file = '/tmp/crosscraft_registrations.json'
        client_ip = request.remote_addr
        
        try:
            if os.path.exists(rate_limit_file):
                with open(rate_limit_file, 'r') as f:
                    registrations = json.load(f)
            else:
                registrations = {}
            
            current_time = datetime.now()
            
            if client_ip in registrations:
                recent_times = [
                    datetime.fromisoformat(t)
                    for t in registrations[client_ip]
                    if datetime.fromisoformat(t) > current_time - timedelta(hours=1)
                ]
                
                if len(recent_times) >= 3:
                    return render_template('register.html', 
                        error='Too many registration attempts. Try again later.'), 429
                
                registrations[client_ip] = [t.isoformat() for t in recent_times] + [current_time.isoformat()]
            else:
                registrations[client_ip] = [current_time.isoformat()]
            
            with open(rate_limit_file, 'w') as f:
                json.dump(registrations, f)
        
        except Exception as e:
            app.logger.warning(f"Rate limiting error: {e}")
        
        # Создаём пользователя (email_confirmed = False)
        new_user = User(
            username=username,
            email=email,
            email_confirmed=False,
            created_at=datetime.utcnow()  # ✅ ВАЖНО
        )
        new_user.set_password(password)
        db.session.add(new_user)
        db.session.commit()
        
        # Отправляем письмо
        if send_confirmation_email(email, username):
            return render_template('register.html',
                success=f'Registration successful! Please check your email ({email}) to confirm your account.')
        else:
            return render_template('register.html',
                error='Registration successful, but email sending failed. Please contact admin.'), 500
    
    return render_template('register.html')

@app.route('/confirm/<token>')
def confirm_email(token):
    try:
        email = confirm_token(token)
    except:
        return render_template('message.html',
            title='Invalid Link',
            message='The confirmation link is invalid or has expired.')
    
    if not email:
        return render_template('message.html',
            title='Invalid Link',
            message='The confirmation link is invalid or has expired.')
    
    user = User.query.filter_by(email=email).first()
    
    if not user:
        return render_template('message.html',
            title='User Not Found',
            message='No user found with this email.')
    
    if user.email_confirmed:
        return render_template('message.html',
            title='Already Confirmed',
            message='Your email has already been confirmed. You can login now.')
    
    user.email_confirmed = True
    user.confirmed_at = datetime.utcnow()
    db.session.commit()
    
    return render_template('message.html',
        title='Email Confirmed!',
        message=f'Thank you {user.username}! Your email has been confirmed. You can now login.')

@app.route('/login.jsp', methods=['GET', 'POST'])
def login():
    if current_user.is_authenticated:
        return redirect(url_for('index'))
    
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        user = User.query.filter_by(username=username).first()
        
        if user and user.check_password(password):
            if not user.email_confirmed:
                flash('Please confirm your email before logging in.')
                return render_template('login.html')
            
            new_session_id = secrets.token_hex(16)
            user.current_session_id = new_session_id
            db.session.commit()
            
            session['_id'] = new_session_id 
            
            login_user(user)
            return redirect(url_for('index'))
        
        flash('Invalid username or password')
    
    return render_template('login.html')

@app.route('/logout')
@login_required
def logout():
    logout_user()
    session.pop('_id', None)
    return redirect(url_for('index'))

@app.route('/about.jsp')
def about():
    return render_template('about.html')

@app.route('/admin')
@login_required
def admin_panel():
    if current_user.id != 1:
        abort(403)
    
    user_count = User.query.count()
    confirmed_count = User.query.filter_by(email_confirmed=True).count()
    unconfirmed_count = User.query.filter_by(email_confirmed=False).count()
    all_users = User.query.order_by(User.id).all()
    
    return render_template('admin.html',
        user_count=user_count,
        confirmed_count=confirmed_count,
        unconfirmed_count=unconfirmed_count,
        all_users=all_users)
    
@app.route('/admin/toggle_license/<int:user_id>', methods=['POST'])
@login_required
def toggle_license(user_id):
    if current_user.id != 1:
        abort(403)
    
    user_to_edit = User.query.get_or_404(user_id)
    user_to_edit.has_license = not user_to_edit.has_license
    db.session.commit()
    flash(f"License for {user_to_edit.username} has been {'granted' if user_to_edit.has_license else 'revoked'}.")
    return redirect(url_for('admin_panel'))


@app.route('/play.jsp')
def serve_play_page():
    # 1. Сбор статистики
    # Используем .count() для быстродействия (вместо len(query.all()))
    try:
        count_servers = Server.query.count()
    except:
        count_servers = 0
        
    try:
        count_users = User.query.count()
    except:
        count_users = 0
        
    # Если у тебя нет отдельной таблицы покупок, можешь считать премиум-юзеров
    # Или просто поставить заглушку, пока не реализуешь
    try:
        # Пример: Purchase.query.count() или User.query.filter_by(is_premium=True).count()
        count_purchases = User.query.filter_by(has_license=True).count()
    except:
        count_purchases = 0

    # 2. Рендеринг шаблона
    # render_template сам подставит current_user, если установлен Flask-Login
    return render_template(
        'game/play.html',  # Файл должен лежать в папке templates/
        total_servers=count_servers,
        total_registered=count_users,
        total_purchased=count_purchases
    )

@app.route('/develope.jsp')
def serve_dev_page():
    return send_from_directory(DEV_DIR, 'game.html')

@app.route('/game.js')
def serve_game_js():
    return send_from_directory(GAME_DIR, 'game.js')

@app.route('/game.wasm')
def serve_game_wasm():
    return send_from_directory(GAME_DIR, 'game.wasm')

@app.route('/game.data')
def serve_game_data():
    return send_from_directory(GAME_DIR, 'game.data')

@app.route('/assets/<path:filename>')
def serve_game_assets(filename):
    return send_from_directory(os.path.join(GAME_DIR, 'assets'), filename)

@app.route('/<path:filename>')
def serve_root_files(filename):
    if filename.endswith(('.png', '.gif')):
        return send_from_directory(GAME_DIR, filename)
    return "Not Found", 404

@app.route('/download/server')
def download_server_software():
    directory = '/root/cc/downloads/servers'
    filename = 's1.5.zip'
    try:
        return send_from_directory(directory, filename, as_attachment=True)
    except FileNotFoundError:
        app.logger.error(f"Attempted to download missing server software: {os.path.join(directory, filename)}")
        abort(404)

@app.route('/listmaps.jsp')
def list_maps():
    username = request.args.get('user')
    if not username:
        return "Parameter 'user' is missing", 400
    
    user = User.query.filter_by(username=username).first()
    if not user:
        return ";".join(["-"] * 5)
    
    output_parts = []
    for slot_id in range(5):
        level = Level.query.filter_by(user_id=user.id, slot=slot_id).first()
        if level:
            output_parts.append(level.name)
        else:
            output_parts.append("-")
    
    response = app.make_response(";".join(output_parts))
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

@app.route('/heartbeat.jsp', methods=['POST'])
def heartbeat():
    try:
        name = request.form.get('name')
        users = int(request.form.get('users', 0))
        max_users = int(request.form.get('max', 0))
        port = int(request.form.get('port', 0))
        is_public_str = request.form.get('public', 'false').lower()
        is_public = is_public_str == 'true'
        ip_address = request.remote_addr

        if not all([name, port > 0, max_users > 0]):
            return "Missing or invalid parameters", 400

    except (ValueError, TypeError):
        return "Invalid parameter format", 400

    server = Server.query.filter_by(ip_address=ip_address, port=port).first()
    
    now = datetime.utcnow()

    if server:
        server.name = name
        server.users = users
        server.max_users = max_users
        server.is_public = is_public
        server.last_heartbeat = now 
    else:
        server = Server(
            name=name,
            users=users,
            max_users=max_users,
            ip_address=ip_address,
            port=port,
            is_public=is_public,
            last_heartbeat=now
        )
        db.session.add(server)
    
    db.session.commit()
    return "OK"

@app.route('/servers.jsp')
def server_list_page():
    cutoff_time = datetime.utcnow() - timedelta(minutes=5)
    Server.query.filter(Server.last_heartbeat < cutoff_time).delete()
    db.session.commit()

    all_live_servers = Server.query.all()
    total_players = sum(s.users for s in all_live_servers)
    total_servers = len(all_live_servers)
    
    public_servers = Server.query.filter_by(is_public=True).order_by(Server.name).all()
    
    return render_template('servers.html', 
                           servers=public_servers,
                           total_players=total_players, 
                           total_servers=total_servers)
    
@app.route('/checkserver.jsp')
def check_server():
    username = request.args.get('user')
    server_id = request.args.get('serverId')
    
    if not username:
        return "NO"
    
    if not server_id or server_id == "":
        return "YES"
    
    user = User.query.filter_by(username=username).first()
    
    if not user:
        app.logger.warning(f"Check failed: User {username} not found")
        return "NO"

    if user.current_session_id == server_id:
        return "YES"
    else:
        app.logger.warning(f"Session mismatch for {username}!")
        app.logger.warning(f"Server sent: {server_id}")
        app.logger.warning(f"DB expects:  {user.current_session_id}")
        return "NO"
    
@app.route('/profile.jsp', methods=['GET', 'POST'])
@login_required
def profile():
    if request.method == 'POST':
        if not current_user.has_license:
            flash("You need a premium license to upload skins.")
            return redirect(url_for('profile'))

        if 'skin' not in request.files:
            flash('No file part')
            return redirect(request.url)
        
        file = request.files['skin']
        
        if file.filename == '':
            flash('No selected file')
            return redirect(request.url)
            
        if file and allowed_file(file.filename):
            filename = f"{current_user.username}.png"
            save_path = os.path.join(app.config['SKIN_FOLDER'], filename)
            
            try:
                file.save(save_path)
                flash('Skin uploaded successfully!')
            except Exception as e:
                app.logger.error(f"Error saving skin: {e}")
                flash('Error saving file.')
                
            return redirect(url_for('profile'))
        else:
            flash('Invalid file type. Please upload PNG or JPG.')
            
    return render_template('profile.html')

@app.route('/skins/<path:username>.png')
def get_skin(username):
    decoded_username = unquote(username)
    
    if '..' in decoded_username or '/' in decoded_username or '\\' in decoded_username:
        return redirect(url_for('static', filename='img/char.png'))

    filename = f"{decoded_username}.png"
    
    try:
        return send_from_directory(app.config['SKIN_FOLDER'], filename)
    except FileNotFoundError:
        return redirect(url_for('static', filename='img/char.png'))
    

@app.route('/resources/<path:filename>')
def serve_resources(filename):
    return send_from_directory(RESOURCES_DIR, filename)    

@app.route('/level/load.html')
def load_level():
    slot_id = request.args.get('id')
    username = request.args.get('user')
    
    if not slot_id or not username:
        return "Missing parameters", 400
    
    user = User.query.filter_by(username=username).first()
    if not user:
        return "User not found", 404
    
    level = Level.query.filter_by(user_id=user.id, slot=int(slot_id)).first()
    
    if not level:
        return "Level not found", 404
    
    try:
        return send_file(level.file_path, mimetype='application/octet-stream', as_attachment=False)
    except FileNotFoundError:
        return "Level file is missing on server", 500

@app.route('/level/save.html', methods=['POST'])
def save_level():
    import sys
    
    try:
        data = request.get_data()
        view = memoryview(data)
        offset = 0
        
        def read_utf():
            nonlocal offset
            length = int.from_bytes(view[offset:offset+2], 'big')
            offset += 2
            value = view[offset:offset+length].tobytes().decode('utf-8')
            offset += length
            return value
        
        username = read_utf()
        sessionid = read_utf()
        levelname = read_utf()
        progress = view[offset]
        offset += 1
        datalen = int.from_bytes(view[offset:offset+4], 'big')
        offset += 4
        slot_id = int.from_bytes(view[offset:offset+4], 'big', signed=True)
        offset += 4
        compressed_map = view[offset:offset+datalen].tobytes()
        
        user = User.query.filter_by(username=username).first()
        if not user:
            return "User not found", 400
        
        if slot_id < 0:
            return "OK"
        
        save_dir = os.path.join(os.getcwd(), "levels", username)
        os.makedirs(save_dir, exist_ok=True)
        
        file_path = os.path.join(save_dir, f"{slot_id}.dat.gz")
        
        with open(file_path, "wb") as f:
            f.write(compressed_map)
        
        level = Level.query.filter_by(user_id=user.id, slot=slot_id).first()
        if level:
            level.name = levelname
            level.file_path = file_path
        else:
            level = Level(
                slot=slot_id,
                name=levelname,
                user_id=user.id,
                file_path=file_path
            )
            db.session.add(level)
        
        db.session.commit()
        return "OK"
    
    except Exception as e:
        import traceback
        traceback.print_exc()
        return f"Server error: {e}", 500
    
    
    
@app.context_processor
def inject_global_stats():
    # 1. Зарегистрированные
    registered_count = User.query.count()
    
    # 2. Купившие (с лицензией)
    purchased_count = User.query.filter_by(has_license=True).count()
    
    # 3. Активные сервера (за последние 5 минут)
    cutoff_time = datetime.utcnow() - timedelta(minutes=5)
    server_count = Server.query.filter(Server.last_heartbeat >= cutoff_time).count()
    
    return dict(
        total_registered=registered_count, 
        total_purchased=purchased_count,
        total_servers=server_count
    )

# ✅ ЗАПУСК ФОНОВОЙ ОЧИСТКИ
with app.app_context():
    db.create_all()
    start_cleanup_scheduler()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000, debug=False)

if __name__ != '__main__':
    handler = RotatingFileHandler('app.log', maxBytes=10*1024*1024, backupCount=3)
    handler.setLevel(logging.INFO)
    app.logger.addHandler(handler)
    app.logger.setLevel(logging.INFO)
    gunicorn_logger = logging.getLogger('gunicorn.error')
    app.logger.handlers = gunicorn_logger.handlers
    app.logger.setLevel(gunicorn_logger.level)
