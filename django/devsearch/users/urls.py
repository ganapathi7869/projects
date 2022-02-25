
from django.urls import path
from . import views 
from django.contrib.auth import views as auth_views

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

    path('inbox/',views.getinbox,name='inbox'),
    path('message/<str:val>/',views.getmessage,name='message'),
    path('create-message/<str:val>/',views.createmessage,name='createmessage'),

    # don't change url names in password reset
    # path('password_reset/',auth_views.PasswordResetView.as_view(),'password_reset'),
    # path('password_reset_sent/',auth_views.PasswordResetDoneView.as_view(),'password_reset_done'),
    # path('password_reset_confirm/',auth_views.PasswordResetConfirmView.as_view(),'password_reset_confirm'),
    # path('password_reset_complete/',auth_views.PasswordResetCompleteView.as_view(),'password_reset_complete'),
]
