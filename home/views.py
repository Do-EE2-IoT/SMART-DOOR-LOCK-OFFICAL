from django.shortcuts import render
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status
from .models import Course
from .serializers import getAllCourseSerializer,CourseSerializer
from django.utils import timezone
from django.views import View
from django.contrib.auth import authenticate,login
from django.http import HttpResponse
from django.contrib.auth.mixins import LoginRequiredMixin
from pytz import timezone as tz



# Create your views here.
class GetAllcourseAPIView(APIView):

    def get(self,request):
        list_course = Course.objects.all()
        mydata =  getAllCourseSerializer(list_course, many = True)
        return Response(data=mydata.data,status=status.HTTP_200_OK)
    
    def post(self,request):
        mydata = CourseSerializer(data = request.data)
        if not mydata.is_valid():
            return Response("sai du lieu roi", status=status.HTTP_400_BAD_REQUEST)
        content = mydata.data['content1']
        Take_post_from_client = Course.objects.create(content =content)
        return Response(data= Take_post_from_client.id,status=status.HTTP_200_OK)


    
class LoginClass(View):
     def get(self,request):
          return render(request, 'home/login_template.html')
     
     def post(self,request):
          password = request.POST.get("password")
          user_name = request.POST.get("user_name")
          my_user = authenticate(username = user_name,password = password)
          if my_user is None:
               return HttpResponse('User khong ton tai')
          
          login(request,my_user)
          return render(request, "home/successful.html")
     
class View_data(View):
     def get(self,request):
          data_from_database = Course.objects.all()
          return render(request,'home/view_data.html',{'data':data_from_database})
     

