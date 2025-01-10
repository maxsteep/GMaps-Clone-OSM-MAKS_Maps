#pragma once


#include "nanoflann.hpp"    //NN library, modified


//Custom data set class, holds a struct of the projected latlons and the dataset to kd-tree adapter 
template <typename T>
struct PointCloud
{
    //public:
    struct Point
    {
        T  x,y;     //x and y are the projected lat lons
    };

    std::vector<Point>  pts;
    
    //dataset to kd-tree below

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
    inline T kdtree_distance(const T *p1, const size_t idx_p2,size_t /*size*/) const
    {
            const T d0=p1[0]-pts[idx_p2].x;
            const T d1=p1[1]-pts[idx_p2].y;
            return d0*d0+d1*d1;
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline T kdtree_get_pt(const size_t idx, int dim) const
    {
            if (dim==0) return pts[idx].x;
            else return pts[idx].y;
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }
    //dataset to kd-tree end
};


//Templating and typedefing the nanoflann interface container
//The container contains the k-d trees and other information for indexing a set of points
//for nearest-neighbor matching.
template <typename num_t>
using my_kd_tree_t = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<num_t, PointCloud<num_t> > ,
        PointCloud<num_t>,
        2 /* dim */
        >
        ;
