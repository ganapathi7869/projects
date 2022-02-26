
from django.urls import path
from . import views

urlpatterns = [
    path('',views.getroutes),
    path('projects/',views.getprojects),
    path('projects/<str:val>/',views.getproject),
]