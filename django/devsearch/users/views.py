from django.shortcuts import render,redirect

# Create your views here.
from .models import Profile
from django.contrib.auth.models import User
from django.contrib import messages
from django.contrib.auth import authenticate,login,logout
from django.contrib.auth.decorators import login_required
from .forms import CustomUserCreationForm, ProfileForm, SkillForm

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

@login_required(login_url='login')
def getaccount(req):
    prof = req.user.profile
    projs = prof.project_set.all()
    skills = prof.skill_set.all()
    cntxt = {'prof':prof, 'skills':skills,'projs':projs}
    return render(req,'users/account.html',cntxt)

@login_required(login_url='login')
def editprofile(req):
    prof = req.user.profile
    if req.method == "POST":
        form = ProfileForm(req.POST, req.FILES, instance=prof)
        if form.is_valid():
            form.save()
            return redirect('account')
    form = ProfileForm(instance=prof)
    cntxt = {'form':form}
    return render(req,'users/profile_form.html',cntxt)

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
            return redirect('editprofile')
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

@login_required(login_url='login')
def createskill(req):
    prof = req.user.profile
    form = SkillForm()

    if req.method == 'POST':
        form = SkillForm(req.POST)
        if form.is_valid():
            skill = form.save(commit=False)
            skill.owner = prof
            skill.save()
            messages.success(req,'Skill added successfully!')
            return redirect('account')
    cntxt = {'form':form}
    return render(req,'users/skill_form.html',cntxt)

@login_required(login_url='login')
def updateskill(req,val):
    prof = req.user.profile
    skill = prof.skill_set.get(id=val)

    if req.method == 'POST':
        form = SkillForm(req.POST, instance=skill)
        if form.is_valid():
            form.save()
            messages.success(req,'Skill updated successfully!')
            return redirect('account')
    form = SkillForm(instance=skill)
    cntxt = {'form':form}
    return render(req,'users/skill_form.html',cntxt)

@login_required(login_url='login')
def deleteskill(req,val):
    prof = req.user.profile
    skill = prof.skill_set.get(id=val)

    if req.method == 'POST':
        skill.delete()
        messages.success(req,'Skill deleted successfully!')
        return redirect('account')
    cntxt = {'object':skill}
    return render(req,'delete_template.html',cntxt)
