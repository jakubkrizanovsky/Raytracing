import json
import random
import sys
import os

def generate_scene(num_spheres, filename):
    scene = {
        "camera": {
            "position": [0, 5, -20],
            "forward": [0, -0.2, 1],
            "fov": 90,
            "focusPoint": [0, 0, 0],
            "orbitVelocity": 0.5
        },
        "lightData": {
            "directionalLightDirection": [-1, -1, 1],
            "directionalLightColor": [0.8, 0.8, 0.8],
            "ambientLightColor": [0.2, 0.2, 0.2]
        },
        "spheres": []
    }

    for _ in range(num_spheres):
        scene["spheres"].append({
            "position": [
                round(random.uniform(-15, 15), 3),
                round(random.uniform(-5, 10), 3),
                round(random.uniform(-15, 15), 3)
            ],
            "radius": round(random.uniform(0.1, 0.8), 3),
            "color": [
                round(random.random(), 3),
                round(random.random(), 3),
                round(random.random(), 3)
            ]
        })

    with open(filename, 'w') as f:
        json.dump(scene, f, indent=4)
    
    print(f"Generated {num_spheres} spheres in: {os.path.relpath(filename)}")

if __name__ == "__main__":
    # Default values
    count = 1000
    name = "scene.json"

    # Override with command line arguments if provided
    if len(sys.argv) > 1:
        try:
            count = int(sys.argv[1])
        except ValueError:
            print("Usage: python3 scene_generator.py [number_of_spheres] [filename]")
            sys.exit(1)
            
    if len(sys.argv) > 2:
        name = sys.argv[2]

    generate_scene(count, name)