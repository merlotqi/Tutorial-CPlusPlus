#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>


int main(int argc, char **argv)
{
    pcl::PointCloud<pcl::PointXYZ> cloud;

    cloud.width = 5;       // 5 points
    cloud.height = 1;      // unorganized cloud, if height > 1, organized cloud
    cloud.is_dense = false;// allow invalid points, e.g. NaNs and Infs are allowed in the cloud data
                           // if true, all points are valid, you can use pcl::isFinite() to check if a point is valid
    cloud.points.resize(cloud.width * cloud.height);

    for (size_t i = 0; i < cloud.points.size(); ++i)
    {
        cloud.points[i].x = 1024 * rand() / (RAND_MAX + 1.0f);
        cloud.points[i].y = 1024 * rand() / (RAND_MAX + 1.0f);
        cloud.points[i].z = 1024 * rand() / (RAND_MAX + 1.0f);
    }

    pcl::io::savePCDFileASCII("test_pcd.pcd", cloud);
    std::cerr << "Saved " << cloud.points.size() << " data points to test_pcd.pcd." << std::endl;

    for (size_t i = 0; i < cloud.points.size(); ++i)
    {
        std::cerr << "    " << cloud.points[i].x << " " << cloud.points[i].y << " " << cloud.points[i].z << std::endl;
    }
    return (0);
}