from .models import Profile, Skill
from django.db.models import Q
from django.core.paginator import Paginator, PageNotAnInteger, EmptyPage

def paginateProfiles(req,profs,resultsperpage):
    paginator = Paginator(profs,resultsperpage)
    pageno = req.GET.get('page')
    try:
        profs = paginator.page(pageno)
    except PageNotAnInteger:
        pageno = 1
        profs = paginator.page(pageno)
    except EmptyPage:
        pageno = paginator.num_pages
        profs = paginator.page(pageno)
    
    leftindex,rightindex = max(1,int(pageno)-4),min(paginator.num_pages,int(pageno)+5)
    customrange  = range(leftindex,rightindex+1)
    return profs,customrange

def searchProfiles(req):
    searchquery = ''
    if req.GET.get('search_query'):
        searchquery = req.GET.get('search_query')
    profs = Profile.objects.filter(
        Q(name__icontains=searchquery) |            # '&' -> and, '|' -> or 
        Q(username__icontains=searchquery) |
        Q(short_intro__icontains=searchquery) |
        Q(skill__name__icontains=searchquery)  #another-way: Q(skill__in=skills) #where: skills=Skill.objects.filter(name__icontains=searchquery)
    ).distinct()
    return profs,searchquery