
from rest_framework.decorators import api_view
from rest_framework.response import Response
from projects.models import Project
from . import serializers

@api_view(['GET'])
def getroutes(req):
    routes = [
        {'GET':'/api/projects'},
        {'GET':'/api/projects/id'},
        {'POST':'/api/projects/id/vote'},

        {'POST':'/api/users/token'},
        {'POST':'/api/users/token/refresh'},
    ]
    return Response(routes)

@api_view(['GET'])
def getprojects(req):
    projs = Project.objects.all()
    serializer = serializers.ProjectSerializer(projs,many=True)
    return Response(serializer.data)

@api_view(['GET'])
def getproject(req,val):
    proj = Project.objects.get(id=val)
    serializer = serializers.ProjectSerializer(proj,many=False)
    return Response(serializer.data)
