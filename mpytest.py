class MyClass(object):
    def __init__(self, *args):
        self.args = args

    def do(self):
        return sum(self.args)
