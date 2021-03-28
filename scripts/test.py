import zen

def execute(frame):
        import zen
        if frame == 0: zen.addNode('ReadParticles', 'No4')
        zen.setNodeParam('No4', 'path', str(f'assets/monkey.obj'))
        zen.applyNode('No4')
        if frame == 0: zen.addNode('ViewParticles', 'No9')
        zen.setNodeInput('No9', 'pars', 'No4::pars')
        zen.applyNode('No9')
        if frame == 0: zen.addNode('EndFrame', 'endFrame')
        zen.applyNode('endFrame')

zen.initialize()
for frame in range(1):
        print('[Zen] executing frame', frame)
        execute(frame)
zen.finalize()
