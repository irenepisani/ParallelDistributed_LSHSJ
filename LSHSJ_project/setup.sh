#!/bin/bash

# Create useful directory 
mkdir -p datasets
mkdir -p dependencies
mkdir -p outputs
mkdir -p logs

# Copy datasets and dependencies
rsync -avz --progress /opt/SPMcode/LSH/datasets/ ./datasets/
rsync -avz --progress /opt/SPMcode/LSH/dependencies/ ./dependencies/

# Download FastFlow (FF) library 
git clone https://github.com/fastflow/fastflow.git

# Run MakeFile
make 
