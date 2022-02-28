
from django.urls import path
from . import views
from rest_framework_simplejwt.views import (
    TokenObtainPairView,
    TokenRefreshView,
)

urlpatterns = [
    path('users/token/', TokenObtainPairView.as_view(), name='token_obtain_pair'),
    path('users/token/refresh/', TokenRefreshView.as_view(), name='token_refresh'),

    path('',views.getroutes),
    path('projects/',views.getprojects),
    path('projects/<str:val>/',views.getproject),
    path('projects/<str:val>/vote/',views.voteproject),
]