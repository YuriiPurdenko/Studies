from random import shuffle, choice, sample, randint

def tree_generator(n): 
    print("(define test-tree-"+str(n))
    print("  (graph ")
    print("    (list", end=" ")
    G = [k for k in range(1,n+1)]
    for v in G: 
        print(str(v),end=" ")
    print(")")
    shuffle(G)
    in_graph = [G[0]] 
    print("    (list ")
    for i in range(1,len(G)): 
        print("      (edge "+str(G[i])+" "+str(choice(in_graph))+")")
        print("      (edge "+str(choice(in_graph))+" "+str(G[i])+")")
        in_graph.append(G[i])
    print(")))")

def graph_generator(n): 
    print("(define test-graph-"+str(n))
    print("  (graph ")
    print("    (list", end=" ")
    G = [k for k in range(1,n+1)]
    for v in G: 
        print(str(v),end=" ")
    print(")")
    shuffle(G) 
    in_graph= [G[0]]
    print("    (list ")
    for i in range(1,len(G)): 
        w = randint(0,i)
        som = sample(in_graph,w)
        for j in som: 
            print("      (edge "+str(G[i])+" "+str(j)+")")
            print("      (edge "+str(j)+" "+str(G[i])+")")
        in_graph.append(G[i])
    print(")))")

tree_generator(5)
print()
graph_generator(5)
print()
tree_generator(10)
print()
graph_generator(10)
print()
tree_generator(15)
print()
graph_generator(15)
print()
tree_generator(25)
print()
graph_generator(25)
print()
