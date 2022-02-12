
from django.urls import path
from . import views 

urlpatterns = [
    path('login/',views.loginuser,name='login'),
    path('logout/',views.logoutuser,name='logout'),
    path('',views.getprofiles,name='profiles'),
    path('profile/<str:val>',views.getprofile,name='profile'),
]
