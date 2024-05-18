from datetime import datetime, timedelta
from flask_cors import CORS
from flask_login import LoginManager
import logging
from .models.models import db
from .app import app, scheduler
from .schemas.schemas import ma
from .routes import api
from .services import skm_service
from datetime import timedelta
import sys

sys.path.append("..")

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s %(levelname)s %(message)s')
logger = logging.getLogger(__name__)

# initialize database with models from models.py
# and marshmallow schemas from schemas.py
db.init_app(app)
ma.init_app(app)

# initialize scheduler

# Run skm service now and periodically every 5 minute
scheduler.add_job(id='skm-service', func=skm_service.update, trigger='interval', minutes=5,
                  next_run_time=datetime.now() + timedelta(seconds=5))

# use CORS to enable frontend access to API
CORS(app)

# register blueprint for api routing
app.register_blueprint(api, url_prefix='/api')

app.secret_key = 'the random string'
