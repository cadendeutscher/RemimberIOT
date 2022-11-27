
from flask import Flask, redirect, render_template, request, url_for, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
import json

app = Flask(__name__)
#Set up DB
SQLALCHEMY_DATABASE_URI = "mysql+mysqlconnector://{username}:{password}@{hostname}/{databasename}".format(
    username="cadendeutscher",
    password="superhuman101",
    hostname="cadendeutscher.mysql.pythonanywhere-services.com",
    databasename="cadendeutscher$iotdb",
)
app.config["SQLALCHEMY_DATABASE_URI"] = SQLALCHEMY_DATABASE_URI
app.config["SQLALCHEMY_POOL_RECYCLE"] = 299
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
db = SQLAlchemy(app)

#Set up the Model for the DB
class Comment(db.Model):

    __tablename__ = "reminders"

    id = db.Column(db.Integer, primary_key=True)
    content = db.Column(db.String(4096))
    date = db.Column(db.String(4096))

#Adds comment when the form is submitted otherwise just displays the comments
@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "GET":
        return render_template("main_template.html", reminders=Comment.query.all())

    comment = Comment(content=request.form["reminder"], date=request.form["date"])
    db.session.add(comment)
    db.session.commit()
    return redirect(url_for('index'))

#Deletes a selected comment
@app.route("/delete/<idd>/", methods=["GET", "POST"])
def delete(idd):
    obj = Comment.query.filter_by(id=idd).one()
    db.session.delete(obj)
    db.session.commit()
    return redirect(url_for('index'))

#Edits a selected comment
@app.route("/edit/<idd>/", methods=["GET", "POST"])
def edit(idd):
    obj = Comment.query.filter_by(id=idd).one()
    obj.content = request.form["reminderc"]
    obj.date = request.form["datec"]
    db.session.commit()
    return redirect(url_for('index'))

#Returns a JSONArray for the reminders that are today
@app.route("/today")
def today():
    #Get Todays Date
    tday = datetime.now().strftime("20%y-%m-%d")
    #Query reminders that are todays date
    tData = Comment.query.filter((Comment.date == tday)).all()
    dList = []
    #For each reminder take the reminder and pplace it in a dicitonary
    for comment in tData:
        dDic = {
            "id": comment.id,
            "content": comment.content,
            "date": comment.date
        }
        dList.append(dDic)
        #Return a JSON array of the reminders
    return json.dumps(dList)
