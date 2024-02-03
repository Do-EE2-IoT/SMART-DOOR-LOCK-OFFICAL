
from django.urls import path
from . import views

urlpatterns = [
    path('login/',views.LoginClass.as_view(),name = "login"),
    path('course/',views.View_data.as_view(),name = "view_Data"),
    path('post_here/',views.GetAllcourseAPIView.as_view(),name = "post_here")
]
