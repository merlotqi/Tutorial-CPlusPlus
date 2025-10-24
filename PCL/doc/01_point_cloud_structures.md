### PCL 核心点类型字段详解

下表列出了PCL中最常见的点类型及其包含的字段。

| 点类型 (Point Type) | 包含字段 | 字段数据类型 | 字段含义与用途 |
| :--- | :--- | :--- | :--- |
| **`pcl::PointXYZ`** | `x, y, z` | `float` | 最基础的点类型，仅包含3D空间坐标。 |
| **`pcl::PointXYZI`** | `x, y, z` | `float` | 3D空间坐标。 |
| | `intensity` | `float` | **反射强度**，来自激光雷达的回波强度信息。 |
| **`pcl::PointXYZRGB`** | `x, y, z` | `float` | 3D空间坐标。 |
| | `rgb` | `float` | 打包的RGB颜色信息。通常通过`r, g, b`三个`uint8_t`成员访问。 |
| **`pcl::PointXYZRGBA`** | `x, y, z` | `float` | 3D空间坐标。 |
| | `rgba` | `uint32_t` | 打包的RGB颜色信息外加透明度(A)。 |
| **`pcl::PointNormal`** | `x, y, z` | `float` | 3D空间坐标。 |
| | `normal_x, normal_y, normal_z` | `float` | 该点对应的**曲面法向量**。 |
| | `curvature` | `float` | 该点处的**曲率**，表示曲面的弯曲程度。 |
| **`pcl::PointXYZL`** | `x, y, z` | `float` | 3D空间坐标。 |
| | `label` | `uint32_t` | 点的**标签**，通常用于存储语义分割或分类的结果。 |

---

### 高级与复合点类型

这些点类型包含了更丰富的信息，用于特定的算法和处理流程。

| 点类型 (Point Type) | 包含字段 | 字段数据类型 | 字段含义与用途 |
| :--- | :--- | :--- | :--- |
| **`pcl::PointXYZINormal`** | `x, y, z` | `float` | 3D空间坐标。 |
| | `intensity` | `float` | 反射强度。 |
| | `normal_x, normal_y, normal_z` | `float` | 曲面法向量。 |
| | `curvature` | `float` | 曲率。 |
| **`pcl::PointXYZRGBNormal`** | `x, y, z` | `float` | 3D空间坐标。 |
| | `rgb` | `float` | 打包的RGB颜色信息。 |
| | `normal_x, normal_y, normal_z` | `float` | 曲面法向量。 |
| | `curvature` | `float` | 曲率。 |

---

### 重要说明与使用技巧

1.  **`rgb` 字段的特殊处理**：

    `rgb` 字段虽然被声明为 `float`，但它实际上是将三个 `uint8_t` (0-255) 的字符打包到一个 `float` 的存储空间中。在代码中，我们通常使用联合体或结构体来直接访问R、G、B分量。

    ```cpp
    pcl::PointXYZRGB p;
    p.r = 255;
    p.g = 0;
    p.b = 0;
    // 此时 p.rgb 存储了一个打包后的值，代表红色
    ```

2.  **点类型的兼容性与大小**：

    *   所有以 `PointXYZ` 开头的点类型在内存布局上都是兼容的，这意味着你有时可以将 `PointXYZI` 的云强制转换为 `PointXYZ` 的云来使用（不推荐在生产环境随意使用）。
    
    *   点类型包含的字段越多，点云所占用的内存就越大。应根据实际应用选择最合适的点类型以平衡性能与功能。

3.  **如何选择点类型？**

    *   **只有几何信息**：使用 `PointXYZ`。
    *   **几何+强度**（来自LiDAR）：使用 `PointXYZI`。
    *   **几何+颜色**（来自RGB相机）：使用 `PointXYZRGB`。
    *   **进行表面重建、分割**：使用带法向量的类型，如 `PointNormal`。
    *   **语义分割结果**：使用 `PointXYZL`。

4.  **自定义点类型**：

    如果标准点类型无法满足需求，PCL允许用户定义自己的点类型，只需包含所需的字段并遵循PCL的命名约定即可。
