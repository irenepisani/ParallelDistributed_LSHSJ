University of Pisa, UNIPI \
M.Sc. Computer science, Artificial Intelligence \
Academic year 2023/2024

# Parallel and Distributed Systems: Paradigms and Models
###### Professors: Massimo Torquati, Marco Danelutto


## [Project 3](Project 3) : 
### Project 3: LSH-based similarity join

  * **Dataset:** [External link](http://download.microsoft.com/download/A/1/1/A116CD80-5B79-407E-B5CE-3D5C6ED8B0D5/msrc_objcategimagedatabase_v1.zip), [Used images](Midterm_1/images_)
  * **Colab Notebook:** [`midterm_1_5.ipynb`](Midterm_1/midterm_1_5.ipynb)
  * **Assignment Overview:** Select four thematic subsets of your choice, out of the total 8 available, and collect all the associated images. For these images, extract the **SIFT descriptors** using the visual feature detector embedded in SIFT to identify the points of interest. Aggregate all the identified descriptors in a dataset and run **k-means** (or any clustering algorithm of your choice) on such data to partition the descriptors in **clusters**. Then analyze the obtained clusters by confronting the descriptors assigned to each cluster with the thematic classes of the images from which they were extracted (in other words, compute a confusion matrix between the clusters and the four thematic images). Discuss your findings. Choice of the number of clusters and of the clustering algorithm is on you (and should be discussed in the report).
