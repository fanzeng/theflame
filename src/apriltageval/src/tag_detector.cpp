#include"tag_detector.h"

TagDetector::TagDetector() {


}

int TagDetector::detect_tag(const std::string filename_image_with_tag) {
    apriltag_detector_t *td = apriltag_detector_create();
    apriltag_family_t *tf = tag36h11_create();
    apriltag_detector_add_family(td, tf);
    td->quad_decimate = 2.0;
    td->quad_sigma = 0.0;
    td->refine_edges = 1;
    td->decode_sharpening = 0.25;

    image_u8_t* image_with_tag_u8_t =image_u8_create_from_pnm(filename_image_with_tag.c_str());
    zarray_t *detections = apriltag_detector_detect(td, image_with_tag_u8_t);
    int detected_tag_id;
    for (int i = 0; i < zarray_size(detections); i++) {
        apriltag_detection_t *det;
        zarray_get(detections, i, &det);
        detected_tag_id = det->id;
    }
    std::cout << "detection finished. " << std::endl;
    apriltag_detections_destroy(detections);
    return detected_tag_id;
}

std::vector<size_t> TagDetector::detect_tag_for_dir(const std::vector<std::string> vec_filename_image_with_tag) {
    std::vector<size_t> vec_detected_tag_id;
    for (std::vector<std::string>::const_iterator it = vec_filename_image_with_tag.begin(); it != vec_filename_image_with_tag.end(); it++) {
        std::string filename_image_with_tag = (*it);
        int detected_tag_id = detect_tag(filename_image_with_tag);
        std::cout << "The detected tag id = " << detected_tag_id << std::endl;
        vec_detected_tag_id.emplace_back(detected_tag_id);
    }
    return vec_detected_tag_id;
}
