
from django.contrib import admin
from django.urls import path

from . import views

urlpatterns = [
    path('', views.getprojects, name='projects'),
    path('project/<str:val>', views.getproject, name='project'),
]
