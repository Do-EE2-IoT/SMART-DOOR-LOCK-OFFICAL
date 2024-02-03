from rest_framework import serializers
from .models import Course
from django.utils import timezone
from pytz import timezone as tz

class getAllCourseSerializer(serializers.ModelSerializer):

    vietnam_time = serializers.SerializerMethodField()

    def get_vietnam_time(self, obj):
        vietnam = tz('Asia/Ho_Chi_Minh')
        return obj.vietnam_time.astimezone(vietnam).strftime("%Y-%m-%d %H:%M:%S")

    class Meta:
        model = Course
        fields = ('content' ,'vietnam_time')


class CourseSerializer(serializers.Serializer):
    content1 = serializers.CharField(max_length = 200)


    
