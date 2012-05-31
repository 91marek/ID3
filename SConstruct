# SConstruct - TODO
### opcje
boost_path='F:/boost_1_49_0'
##
import sys
env = Environment(CPPPATH=['.', boost_path])
if ARGUMENTS.get('debug'):
	env.Append(CPPDEFINES='DEBUG')
if sys.platform == 'win32':
	env.Append(CXXFLAGS='/EHsc')


env.StaticLibrary('id3lib/id3lib', ['id3lib/DecisionTree.cpp', 'id3lib/Table.cpp', 'id3lib/TrainingSet.cpp', 'id3lib/UMDecisionTree.cpp', 'id3lib/ZPRDecisionTree.cpp'])
env.Program('id3', 'id3app/main.cpp', LIBS=['id3lib'], LIBPATH=[ './id3lib'])