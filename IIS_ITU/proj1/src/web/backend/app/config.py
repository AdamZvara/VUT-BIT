import os

basedir = os.path.abspath(os.path.dirname(__file__))

class Config(object):
    SQLALCHEMY_DATABASE_URI = os.getenv("DATABASE_URL", "sqlite://")
    SQLALCHEMY_TRACK_MODIFICATIONS = False
    MEDIA_FOLDER = f"{os.getenv('APP_FOLDER')}/app/media"
    SCHEDULER_API_ENABLED = True
    SCHEDULER_TIMEZONE = "Europe/Berlin"  # <========== add here
