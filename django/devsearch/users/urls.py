
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

    # don't change url names for password reset
    path('password-reset/',auth_views.PasswordResetView.as_view(template_name='users/password_reset.html'),name='password_reset'),
    path('password-reset-sent/',auth_views.PasswordResetDoneView.as_view(template_name='users/password_reset_sent.html'),name='password_reset_done'),
    path('password-reset-confirm/<uidb64>/<token>/',auth_views.PasswordResetConfirmView.as_view(template_name='users/password_reset_confirm.html'),name='password_reset_confirm'),
    path('password-reset-complete/',auth_views.PasswordResetCompleteView.as_view(template_name='users/password_reset_complete.html'),name='password_reset_complete'),
]
