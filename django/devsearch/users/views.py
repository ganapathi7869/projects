from django.shortcuts import render

# Create your views here.
from .models import Profile

def getprofiles(req):
    profs = Profile.objects.all()
    cntxt = {"profs":profs}
    return render(req,'users/profiles.html',cntxt)

def getprofile(req,val):
    prof = Profile.objects.get(id=val)
    topskills = prof.skill_set.exclude(description__exact='')
    otherskills = prof.skill_set.filter(description='')
    cntxt = {'prof':prof,'topskills':topskills,'otherskills':otherskills}
    return render(req,'users/profile.html',cntxt)
