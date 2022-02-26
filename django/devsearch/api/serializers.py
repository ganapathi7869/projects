
from rest_framework import serializers
from projects.models import Project, Review, Tag
from users.models import Profile

class ProfileSerializer(serializers.ModelSerializer):
    class Meta:
        model = Profile
        fields = '__all__'

class TagSerializer(serializers.ModelSerializer):
    class Meta:
        model = Tag
        fields = '__all__'

class ReviewSerializer(serializers.ModelSerializer):
    class Meta:
        model = Review
        fields = '__all__'

class ProjectSerializer(serializers.ModelSerializer):
    owner = ProfileSerializer(many=False)
    tags = TagSerializer(many=True)
    reviews = serializers.SerializerMethodField()  # custom variable; django calls -> get_<variable_name>() - function
    class Meta:
        model = Project
        fields = '__all__'
    
    # automatically called by django to fill - reviews
    def get_reviews(self,obj):  # obj - object which is getting serialized
        revws = obj.review_set.all()
        serializer = ReviewSerializer(revws,many=True)
        return serializer.data

