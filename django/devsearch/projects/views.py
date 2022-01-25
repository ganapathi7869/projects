import http
import django
from django.shortcuts import render

# Create your views here.
from django.http import HttpResponse
from django.template import context

projectslist=[
    {
        'id': '1',
        'title': 'Ecommerce',
        'description': 'sample description for ecommerce'
    },
    {
        'id': '2',
        'title': 'Portfolio',
        'description': 'sample description for Portfolio'
    },
    {
        'id': '3',
        'title': 'Social Network',
        'description': 'sample description for Social Network'
    },
]

def getprojects(req):
    page='Projects'
    number=10
    cntxt={'page':page,'number':number,'projectslist':projectslist}
    return render(req,'projects/projects.html',cntxt)

def getproject(req,val):
    proj=None
    for i in projectslist:
        if i['id'] == val:
            proj=i
    return render(req,'projects/single-project.html',{'project':proj})