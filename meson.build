project('Blackjack', 'cpp',
        version : '0.0.1',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

sources = [
    'Source/main.cpp'
]

incdir = include_directories('Types', 'Source')

Blackjack = executable('Blackjack', sources, install : true, include_directories : incdir)