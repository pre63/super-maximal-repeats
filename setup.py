import pybind11
from setuptools import Extension, setup

ext = Extension(
  "supermaxrep.smr",
  sources=["supermaxrep/smr.cpp"],
  include_dirs=[pybind11.get_include()],
  language="c++",
  extra_compile_args=["-std=c++11", "-O3"],
)

setup(
  name="supermaxrep",
  version="0.1.0",
  description="Library for computing super-maximal repeats",
  packages=["supermaxrep"],
  ext_modules=[ext],
  install_requires=["pybind11"],
)
