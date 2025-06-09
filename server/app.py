from flask import Flask, request, jsonify, render_template
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from config import Config

app = Flask(__name__)
app.config.from_object(Config)
db = SQLAlchemy(app)

class PullupRecord(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    reps = db.Column(db.Integer, nullable=False)
    timestamp = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)

    def to_dict(self):
        return {
            'id': self.id,
            'reps': self.reps,
            'timestamp': self.timestamp.isoformat()
        }

# Create database tables
with app.app_context():
    db.create_all()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/api/reps', methods=['POST'])
def add_reps():
    data = request.get_json()
    
    if not data or 'reps' not in data:
        return jsonify({'error': 'Missing reps data'}), 400
    
    try:
        reps = int(data['reps'])
        record = PullupRecord(reps=reps)
        db.session.add(record)
        db.session.commit()
        
        return jsonify(record.to_dict()), 201
    except ValueError:
        return jsonify({'error': 'Invalid reps value'}), 400
    except Exception as e:
        db.session.rollback()
        return jsonify({'error': str(e)}), 500

@app.route('/api/reps', methods=['GET'])
def get_all_reps():
    records = PullupRecord.query.order_by(PullupRecord.timestamp.desc()).all()
    return jsonify([record.to_dict() for record in records])

@app.route('/api/reps/today', methods=['GET'])
def get_today_reps():
    today = datetime.utcnow().date()
    records = PullupRecord.query.filter(
        db.func.date(PullupRecord.timestamp) == today
    ).all()
    
    total_reps = sum(record.reps for record in records)
    return jsonify({
        'date': today.isoformat(),
        'total_reps': total_reps,
        'records': [record.to_dict() for record in records]
    })

if __name__ == '__main__':
    app.run(
        host=app.config['HOST'],
        port=app.config['PORT'],
        debug=app.config['DEBUG']
    ) 