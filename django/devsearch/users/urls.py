
from django.urls import path
from . import views 

urlpatterns = [
    path('',views.getprofiles,name='profiles'),
    path('profile/<str:val>',views.getprofile,name='profile'),
]
