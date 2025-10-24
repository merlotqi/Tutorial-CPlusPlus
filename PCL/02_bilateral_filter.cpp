#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>


typedef pcl::PointXYZI PointI;

float G(float x, float sigma)
{
    return exp(-(x * x) / (2 * sigma * sigma));
}

int main(int argc, char **argv)
{
    std::string incloudfile = argv[1];
    std::string outcloudfile = argv[2];
    float sigma_s = atof(argv[3]);
    float sigma_r = atof(argv[4]);
    
    pcl::PointCloud<PointI>::Ptr cloud(new pcl::PointCloud<PointI>);
    pcl::io::loadPCDFile(incloudfile, *cloud);
    int pnumber = (int)cloud->size();

    // output cloud = input cloud
    pcl::PointCloud<PointI> outcloud = *cloud;

    pcl::KdTreeFLANN<PointI> kdtree;
    kdtree.setInputCloud(cloud);

    std::vector<int> knn_indices;
    std::vector<float> knn_sqr_distances;

   for (int point_id = 0; point_id < pnumber; ++point_id)
    {
        float BF = 0;
        float W = 0;

        kdtree.radiusSearch(point_id, 2 * sigma_s, knn_indices, knn_sqr_distances);

        for(std::size_t n_id = 0; n_id < knn_indices.size(); ++n_id)
        {
            float id = knn_indices.at(n_id);
            float dist
             = sqrt(knn_sqr_distances.at(n_id));
            float intensity_dist = std::abs(cloud->points[point_id].intensity - cloud->points[id].intensity);

            float w_a = G(dist, sigma_s);
            float w_b = G(intensity_dist, sigma_r);
            float weight = w_a * w_b;

            BF += cloud->points[id].intensity * weight;
            W += weight;
        }

        outcloud.points[point_id].intensity = BF / W;
    }

    pcl::io::savePCDFileBinary(outcloudfile, outcloud);
    return 0;
}