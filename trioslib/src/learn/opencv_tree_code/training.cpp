#include <cstdio>

#include "opencv2/opencv.hpp"
#include "opencv2/ml/ml.hpp"
#include "trios_mtm.h"

cv::Mat build_classes_from_mtm(mtm_t *mtm) {
    int width = 1;
    int height = mtm->nsum;
    cv::Mat classes(height, width, CV_32FC1);
    int k = 0;
    for (int i = 0; i < mtm->nmtm; i++) {
        mtm_GX *m = (mtm_GX *) mtm->mtm_data + i;
        for (int j = 0; j < m->fq; j++, k++) {

            classes.at<float>(k, 0) = m->label;

        }
    }

    printf("k %d height %d\n", k, height);

    return classes;
}

cv::Mat build_samples_from_mtm(mtm_t *mtm) {
    int width = mtm->wsize;
    int height = mtm->nsum;
    cv::Mat samples(height, width, CV_32FC1);

    for (int i = 0, k = 0; i < mtm->nmtm; i++) {
        mtm_GX *m = (mtm_GX *) mtm->mtm_data + i;
        for (int j = 0; j < m->fq; j++, k++) {

            for (int l = 0; l < mtm->wsize; l++) {
                samples.at<float>(k, l) = (unsigned char) m->wpat[j];
            }

        }
    }
    return samples;
}

void test_accuracy(CvDTree &tr, cv::Mat samples, cv::Mat labels) {
    cv::Mat wpat(1, samples.cols, CV_32FC1);
    unsigned long right, wrong, mse;
    right = wrong = 0, mse;
    for (int i = 0; i < samples.rows; i++) {
        //printf("Sample\n");
        for (int j = 0; j < samples.cols; j++) {
            //printf("%f ", samples.at<float>(i, j));
            wpat.at<float>(0, j) = samples.at<float>(i, j);
        }

        CvDTreeNode *node = tr.predict(wpat);
        if (node->value == labels.at<int>(i, 0)) {
            //printf("\n Label %d Prediction %f\n", labels.at<int>(i, 0), node->value);
            right++;
        } else {
            mse += pow(node->value - labels.at<int>(i, 0), 2);
            //printf("\n Label %d Prediction %f\n", labels.at<int>(i, 0), node->value);
            wrong++;
        }
    }
    printf("Acc %f MSE %lu\n", 1.0 * right/(right + wrong), mse);
}

extern "C" void *ltrainClassifier(mtm_t *mtm) {
    cv::Mat samples_mtm = build_samples_from_mtm(mtm);
    cv::Mat labels_mtm = build_classes_from_mtm(mtm);
    CvDTreeParams params;
    params.truncate_pruned_tree = false;
    CvDTree *tr_mtm = new CvDTree();
    tr_mtm->train(samples_mtm, CV_ROW_SAMPLE, labels_mtm, cv::Mat(), cv::Mat(), cv::Mat(), cv::Mat(), params);
    tr_mtm->pruned_tree_idx = -INT_MAX;

    return (void *) tr_mtm;
}
