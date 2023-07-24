
# Source files
set(sources	
	"Emitter.hpp"
	"Emitter.cpp"

	"KDTree.hpp"
	"KDTree.cpp"

	"Particle.hpp"
	"Particle.cpp"

	"particle_system.hpp"
	"particle_system.cpp"

	"CMakeLists.txt"
)

# Add these sources to the project target
target_relative_sources(${CGRA_PROJECT} ${sources})
