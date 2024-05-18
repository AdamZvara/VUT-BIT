from functools import wraps
from flask import jsonify, request, current_app
from .enums import UserRoleEnum
import jwt

from .models.models import User, db
from .schemas.schemas import user_schema

invalid_msg = {
    'message': 'Invalid token. Registeration and / or authentication required',
    'authenticated': False
}

expired_msg = {
    'message': 'Expired token. Reauthentication required.',
    'authenticated': False
}

insufficient_permission_msg = {
    'message': 'Insufficient permissions',
    'authenticated': False
}

def token_required(f):
    @wraps(f)
    def _verify(*args, **kwargs):
        auth_headers = request.headers.get('Authorization', '').split()

        if len(auth_headers) != 2:
            return jsonify(invalid_msg), 401

        try:
            token = auth_headers[1]
            data = jwt.decode(token, current_app.config['SECRET_KEY'], algorithms="HS256")
            user = db.session.execute(db.select(User).where(User.email == data['sub']))
            user = user_schema.dump(user.scalars().first())
            if not user:
                raise RuntimeError('User not found')
            return f(user, *args, **kwargs)
        except jwt.ExpiredSignatureError:
            return jsonify(expired_msg), 498 # 498 is Token expired/invalid
        except (jwt.InvalidTokenError, Exception) as e:
            print(e)
            return jsonify(invalid_msg), 401 # 401 is Unauthorized HTTP status code

    return _verify

def admin_required(f):
    @wraps(f)
    def _verify_admin(*args, **kwargs):
        auth_headers = request.headers.get('Authorization', '').split()

        if len(auth_headers) != 2:
            return jsonify(invalid_msg), 401

        try:
            token = auth_headers[1]
            data = jwt.decode(token, current_app.config['SECRET_KEY'], algorithms="HS256")
            user = db.session.execute(db.select(User).where(User.email == data['sub']))
            user = user_schema.dump(user.scalars().first())
            if not user:
                raise RuntimeError('User not found')
            elif data['role'] != UserRoleEnum.admin:
                return jsonify(insufficient_permission_msg), 403 # 403 is Forbidden status code
            return f(user, *args, **kwargs)
        except jwt.ExpiredSignatureError:
            return jsonify(expired_msg), 401 # 401 is Unauthorized HTTP status code
        except (jwt.InvalidTokenError, Exception) as e:
            print(e)
            return jsonify(invalid_msg), 401

    return _verify_admin

def moderator_required(f):
    @wraps(f)
    def _verify_moderator(*args, **kwargs):
        auth_headers = request.headers.get('Authorization', '').split()

        if len(auth_headers) != 2:
            return jsonify(invalid_msg), 401

        try:
            token = auth_headers[1]
            data = jwt.decode(token, current_app.config['SECRET_KEY'], algorithms="HS256")
            user = db.session.execute(db.select(User).where(User.email == data['sub']))
            user = user_schema.dump(user.scalars().first())
            if not user:
                raise RuntimeError('User not found')
            elif data['role'] != UserRoleEnum.mod :
                return jsonify(insufficient_permission_msg), 403 # 403 is Forbidden status code
            return f(user, *args, **kwargs)
        except jwt.ExpiredSignatureError:
            return jsonify(expired_msg), 401 # 401 is Unauthorized HTTP status code
        except (jwt.InvalidTokenError, Exception) as e:
            print(e)
            return jsonify(invalid_msg), 401

    return _verify_moderator

def get_user_credentials(request):
    data = request.get_json(force=True)
    if data is None:
        email = request.values[1]["email"]
        password = request.values[1]["password"]
    else:
        email = request.json.get('email')
        password = request.json.get('password')
    return email, password

def get_user_login(request):
    data = request.get_json(force=True)
    return request.values[1]["login"] if data is None else request.json.get('login')

def get_user_role(request):
    data = request.get_json(force=True)
    return request.values[1]["role"] if data is None else request.json.get('role')

def get_user_blocked(request):
    data = request.get_json(force=True)
    return request.values[1]["blocked"] if data is None else request.json.get('blocked')

def get_user_newemail(request):
    data = request.get_json(force=True)
    return request.values[1]["new_email"] if data is None else request.json.get('new_email')