create_simplecpp_module(SimpleDG INTERFACE
        # Base Dependency Graph
        include/sdg/DependencyGraph.h
)

link_simplecpp_module(SimpleDG INTERFACE SimpleUtils)
