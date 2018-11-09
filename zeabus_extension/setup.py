from distutils.core import setup

print("----------------------------------------------run setup python")

setup(name='zeabus_python_package',
      version='1.1',
      description='Export Library ROS for zeabus',
      author='Supasan Komonlit',
      author_email='supasan.k@ku.th',
	  maintainer='GAP',
      maintainer_email='supasan_224@hotmail.com',
      url='https://www.facebook.com/SupasanKomonlit',
	  license='BSDC',
      download_url="don't already",
	  long_description='don\'t have',
      packages=['zeabus_extension'],
	  package_dir={ 'zeabus_extension' : 'scripts'},
	  package_data={'zeabus_extension' : ['__init__.py']}
     )
