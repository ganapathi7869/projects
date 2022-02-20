
from django.shortcuts import redirect, render

# Create your views here.
from .models import Project
from .forms import ProjectForm
from django.contrib.auth.decorators import login_required

def getprojects(req):
    projs = Project.objects.all()
    cntxt={'projs':projs}
    return render(req,'projects/projects.html',cntxt)

def getproject(req,val):
    proj=Project.objects.get(id=val)
    return render(req,'projects/single-project.html',{'proj':proj})

@login_required(login_url='login')
def createproject(req):
    prof = req.user.profile
    form = ProjectForm()
    if req.method == 'POST':
        form = ProjectForm(req.POST, req.FILES)
        if form.is_valid():
            proj = form.save(commit=False)
            proj.owner = prof
            proj.save()
            return redirect('account')
    cntxt = {'form': form}
    return render(req,'projects/project_form.html', cntxt)

@login_required(login_url='login')
def updateproject(req,val):
    prof = req.user.profile
    proj = prof.project_set.get(id=val)
    form = ProjectForm(instance=proj)
    if req.method == 'POST':
        # print(req)
        form = ProjectForm(req.POST, req.FILES, instance=proj)
        if form.is_valid():
            # print('valid')
            form.save()
            return redirect('account')
    cntxt = {'form': form}
    return render(req,'projects/project_form.html', cntxt)

@login_required(login_url='login')
def deleteproject(req,val):
    prof = req.user.profile
    proj = prof.project_set.get(id=val)
    if req.method == 'POST':
        proj.delete()
        return redirect('account')
    cntxt = {'object': proj}
    return render(req,'delete_template.html', cntxt)
    