
from django.shortcuts import redirect, render

# Create your views here.
from django.http import HttpResponse
from django.template import context

from .models import Project
from .forms import ProjectForm

def getprojects(req):
    projs = Project.objects.all()
    cntxt={'projs':projs}
    return render(req,'projects/projects.html',cntxt)

def getproject(req,val):
    proj=Project.objects.get(id=val)
    return render(req,'projects/single-project.html',{'proj':proj})

def createproject(req):
    form = ProjectForm()
    if req.method == 'POST':
        form = ProjectForm(req.POST, req.FILES)
        if form.is_valid():
            form.save()
            return redirect('projects')
    cntxt = {'form': form}
    return render(req,'projects/project_form.html', cntxt)

def updateproject(req,val):
    proj = Project.objects.get(id=val)
    form = ProjectForm(instance=proj)
    if req.method == 'POST':
        # print(req)
        form = ProjectForm(req.POST, req.FILES, instance=proj)
        if form.is_valid():
            # print('valid')
            form.save()
            return redirect('projects')
    cntxt = {'form': form}
    return render(req,'projects/project_form.html', cntxt)

def deleteproject(req,val):
    proj = Project.objects.get(id=val)
    if req.method == 'POST':
        proj.delete()
        return redirect('projects')
    cntxt = {'object': proj}
    return render(req,'projects/delete_template.html', cntxt)
    