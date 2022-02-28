
from rest_framework.decorators import api_view, permission_classes
from rest_framework import permissions
from rest_framework.response import Response
from projects.models import Project, Review
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
# @permission_classes([permissions.IsAuthenticated])
def getprojects(req):
    projs = Project.objects.all()
    serializer = serializers.ProjectSerializer(projs,many=True)
    return Response(serializer.data)

@api_view(['GET'])
def getproject(req,val):
    proj = Project.objects.get(id=val)
    serializer = serializers.ProjectSerializer(proj,many=False)
    return Response(serializer.data)

@api_view(['POST'])
@permission_classes([permissions.IsAuthenticated])
def voteproject(req,val):
    proj = Project.objects.get(id=val)
    user = req.user.profile
    data = req.data
    review, created = Review.objects.get_or_create(
        project = proj,
        owner = user
    )
    if review.project.owner == review.owner:
        review.delete()
    else:
        review.value = data['value']
        review.save()
    proj.updatevotes()
    serializer = serializers.ProjectSerializer(proj,many=False)
    return Response(serializer.data)