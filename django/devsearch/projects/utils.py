
from .models import Project,Tag
from django.db.models import Q
from django.core.paginator import Paginator, PageNotAnInteger, EmptyPage

def paginateProjects(req,projs,resultsperpage):
    paginator = Paginator(projs,resultsperpage)
    pageno = req.GET.get('page')
    try:
        projs = paginator.page(pageno)
    except PageNotAnInteger:
        pageno = 1
        projs = paginator.page(pageno)
    except EmptyPage:
        pageno = paginator.num_pages
        projs = paginator.page(pageno)
    
    leftindex,rightindex = max(1,int(pageno)-4),min(paginator.num_pages,int(pageno)+5)
    customrange  = range(leftindex,rightindex+1)
    return projs,customrange

def searchProjects(req):
    searchquery = ''
    if req.GET.get('search_query'):
        searchquery = req.GET.get('search_query')
    projs = Project.objects.filter(
        Q(owner__name__icontains=searchquery) |     # '&' -> and, '|' -> or 
        Q(title__icontains=searchquery) |
        Q(description__icontains=searchquery) |
        Q(tags__name__icontains=searchquery)  #another-way: Q(tags__in=tgs) #where: tgs=Tag.objects.filter(name__icontains=searchquery)
    ).distinct()
    return projs,searchquery