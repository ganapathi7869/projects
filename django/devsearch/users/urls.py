
from django.urls import path
from . import views 

urlpatterns = [
    path('register/',views.registeruser,name='register'),
    path('login/',views.loginuser,name='login'),
    path('logout/',views.logoutuser,name='logout'),

    path('',views.getprofiles,name='profiles'),
    path('profile/<str:val>/',views.getprofile,name='profile'),
    path('account/',views.getaccount,name='account'),
    path('edit-profile/',views.editprofile,name='editprofile'),

    path('create-skill/',views.createskill,name='createskill'),
    path('update-skill/<str:val>/',views.updateskill,name='updateskill'),
    path('delete-skill/<str:val>/',views.deleteskill,name='deleteskill'),
]
