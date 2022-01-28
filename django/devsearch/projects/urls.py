
from django.contrib import admin
from django.urls import path

from . import views

urlpatterns = [
    path('', views.getprojects, name='projects'),
    path('project/<str:val>', views.getproject, name='project'),
    path('create-project', views.createproject, name='createproject'),
    path('update-project/<str:val>', views.updateproject, name='updateproject'),
    path('delete-project/<str:val>', views.deleteproject, name='deleteproject'),
]
