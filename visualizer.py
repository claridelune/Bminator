import json
import networkx as nx
import matplotlib.pyplot as plt
from networkx.drawing.nx_agraph import graphviz_layout

with open("ASTTree.json", "r") as file:
    data = json.load(file)

G = nx.DiGraph()

for node in data:
    if 'id' in node:
        G.add_node(node['id'], label=node['content'], tooltip=node['type'])
    if 'parent' in node and node['parent']:
        G.add_edge(node['parent'], node['id'])

plt.figure(figsize=(16, 10))
pos = graphviz_layout(G, prog="dot")

nx.draw(G, pos, node_size=2000, node_color="skyblue", font_size=9, font_weight="bold", arrows=True)

labels = nx.get_node_attributes(G, 'label')
nx.draw_networkx_labels(G, pos, labels=labels, font_size=8, verticalalignment="top", font_color="black")

# for node, (x, y) in pos.items():
#     plt.text(x, y, f"({G.nodes[node]['tooltip']})", fontsize=7, ha="center", color="gray")

plt.title("AST Tree Visualization with NetworkX")
plt.show()
