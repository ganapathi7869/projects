
from pyexpat.errors import messages
from django.shortcuts import redirect, render

# Create your views here.
from .models import Project, Review
from .forms import ProjectForm,ReviewForm
from django.contrib.auth.decorators import login_required
from django.contrib import messages
from .utils import searchProjects, paginateProjects

def getprojects(req):
    projs ,searchquery = searchProjects(req)
    projs, customrange = paginateProjects(req,projs,6)
    cntxt={'projs':projs,'searchquery':searchquery,'customrange':customrange}
    return render(req,'projects/projects.html',cntxt)

def getproject(req,val):
    proj=Project.objects.get(id=val)
    if req.method == 'POST':
        form = ReviewForm(req.POST)
        review = form.save(commit=False)
        review.owner = req.user.profile
        review.project = proj
        review.save()
        proj.updatevotes()
        messages.success(req,'Your review was successfully submitted!')
        return redirect('project',val=proj.id)
    form = ReviewForm()
    return render(req,'projects/single-project.html',{'proj':proj,'form':form})

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
    