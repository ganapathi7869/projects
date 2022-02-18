from django.shortcuts import render,redirect

# Create your views here.
from .models import Profile
from django.contrib.auth.models import User
from django.contrib import messages
from django.contrib.auth import authenticate,login,logout
from .forms import CustomUserCreationForm

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

def registeruser(req):
    page='register'
    if req.method=='POST':
        form = CustomUserCreationForm(req.POST)
        if form.is_valid():
            user=form.save(commit=False)
            user.username=user.username.lower()
            user.save()
            messages.success(req,'User was created successfully!')
            login(req,user)
            return redirect('profiles')
        else:
            messages.error(req,'An error has occurred while registering the user!')
    form=CustomUserCreationForm()
    cntxt={'form':form,'page':page}
    return render(req,'users/login_register.html',cntxt)

def loginuser(req):
    page='login'
    if req.user.is_authenticated:
        return redirect('profiles')

    if req.method=='POST':
        username,password = req.POST['un'],req.POST['pw']
        try:
            user = User.objects.get(username=username)
        except:
            messages.error(req,"User Does not exist!")
        
        user = authenticate(req,username=username,password=password)
        if user is None:
            messages.error(req,"Username (or) password is incorrect!")
        else:
            login(req,user)
            return redirect('profiles')
    cntxt={'page':page}
    return render(req,'users/login_register.html',cntxt)

def logoutuser(req):
    logout(req)
    messages.info(req,"User logged out Successfully!")
    return redirect('login')