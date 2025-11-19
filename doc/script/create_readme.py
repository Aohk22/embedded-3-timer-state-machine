readme: str
stub = open('../README.stub.md', 'r').read().strip()
contentTodo = open('../to-do.md', 'r').read().strip()

readme = stub.replace('{todo}', contentTodo)

with open('../../README.md', 'w') as f:
    f.write(readme)