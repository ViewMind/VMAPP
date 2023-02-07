#include "ballpathgenerator.h"

BallPathGenerator::BallPathGenerator()
{

}

QList< QList<qint32> > BallPathGenerator::GenerateBallPathSequence(qint32 rows, qint32 cols,
                                                                   qint32 times_per_hole, qint32 n_of_passes,
                                                                   qreal min_distance, qreal mean_distance){

    SimpleMatrix counts;
    counts.clear(rows,cols,1);
    counts = counts.scaleBy(times_per_hole);

    QList< QList<qint32> > ans;

    QList<RowColIndex> sequence_2 = GenerateSequence(counts);
    QList<RowColIndex> sequence_3 = GenerateSequence(counts);
    QList<RowColIndex> sequence_4 = GenerateSequence(counts);
    QList<RowColIndex> sequence_1 = GenerateSequenceConditioned(counts,sequence_2,min_distance,mean_distance);

    qint32 N = sequence_1.size();

    // We only do the passes thing if it's greater than zero.
    if (n_of_passes > 0){

        SimpleMatrix passes_array;
        passes_array.clear(1,2,0);
        passes_array.set(0,0,N-n_of_passes);
        passes_array.set(0,1,n_of_passes);

        QList<RowColIndex> passes = GenerateSequence(passes_array);

        for (qint32 i = 0; i < passes.size(); i++){
            //qDebug() << i << ":" << passes.at(i).toString();
            if (passes.at(i).i < 1){
                sequence_1[i] = sequence_2[i];
            }
        }
    }

    // Creating the final sequence
    for (qint32 i = 0; i < N; i++){
        QList<qint32> trial;
        trial << sequence_1.at(i).getIndex(rows);
        trial << sequence_2.at(i).getIndex(rows);
        trial << sequence_3.at(i).getIndex(rows);
        trial << sequence_4.at(i).getIndex(rows);

        ans << trial;
    }

    return ans;

}

QList<BallPathGenerator::RowColIndex> BallPathGenerator::GenerateSequenceConditioned(const SimpleMatrix &input_counts,
                                                                                     const QList<RowColIndex> &sequence,
                                                                                     qreal min_distance,
                                                                                     qreal target_mean,
                                                                                     qreal eps){

    qreal mean = 0;
    QList<RowColIndex> output;
    QList<qreal> distances;
    qint32 N = sequence.size();
    SimpleMatrix counts;
    counts.fill(input_counts.getData());
    SimpleMatrix::MatDim md = counts.getDim();

    qint32 rows = md.nrows;
    qint32 cols = md.ncols;

    for (qint32 i = 0; i < N; i++){

        qint32 y = sequence.at(i).r;
        qint32 x = sequence.at(i).c;

        SimpleMatrix distance, counts_masked;
        distance.clear(rows,cols,0);
        counts_masked.fill(counts.getData());


        // Apply distance rule by making zero the counter of positions closer
        // than min_distance
        for (qint32 r = 0; r < rows; r++){
            for (qint32 c = 0; c < cols; c++){
                qreal v = qSqrt((c-x)*(c-x) + (r-y)*(r-y));
                distance.set(r,c,v);
                if (v < min_distance){
                    counts_masked.set(r,c,0);
                }
            }
        }

        // In case that not all counts were zeroed, we scale probabilities so it is
        // more likely that a value that makes the mean closer to target_mean
        // will be chosen

        if (counts_masked.sumForceInt() > 0){
            qreal di = static_cast<qreal>(i);
            qreal obj_value = target_mean*di - mean*(di-1);

            for (qint32 r = 0; r < rows; r++){
                for (qint32 c = 0; c < cols; c++){
                    qreal scale = 1/(eps + qAbs(distance.get(r,c) - obj_value));
                    counts_masked.set(r,c,counts_masked.get(r,c)*scale);
                }
            }


        }

        else {

            // If all remaining counts were zeroed (it happens at the end of the
            // sequence or when min_distance is high), then make all positions
            // equiprobable

            counts_masked.clear(rows,cols,1);

        }

        qreal count_masked_sum = counts_masked.sum();
        SimpleMatrix probs;
        probs.clear(rows,cols,0);
        for (qint32 r = 0; r < rows; r++){
            for (qint32 c = 0; c < cols; c++){
                probs.set(r,c,counts_masked.get(r,c)/count_masked_sum);
            }
        }

        RowColIndex rci = BallPathGenerator::Choice(probs);

        // Save current distance to calculate current mean distance
        distances << qSqrt((x-rci.c)*(x-rci.c) + (y - rci.r)*(y - rci.r));
        mean = BallPathGenerator::MeanOnList(distances);

        // This if is to consider the case when all remaining counts were zeroed
        if (counts.get(rci.r,rci.c) > 0){
            counts.decreaseBy1(rci.r,rci.c);
        }

        output << rci;

    }

    return output;

}

QList<BallPathGenerator::RowColIndex> BallPathGenerator::GenerateSequence(const SimpleMatrix &input_counts){

    SimpleMatrix counts;
    counts.fill(input_counts.getData());
    // Input is a 1 by N array.
    qint32 cols = counts.getDim().ncols;
    qint32 rows = counts.getDim().nrows;

    QList<RowColIndex> outputs;

    SimpleMatrix probs;

    while (counts.sumForceInt() > 0){ // We needed it as an int here to ensure a perfect zero.

        //qDebug() << counts.sumForceInt();

        probs.clear(rows,cols,0);

        qreal counts_sum = counts.sum(); // We needed it here as double for the double division in the following steps.

        for (qint32 i = 0; i < rows; i++){
            for (qint32 j = 0; j < cols; j++){
                probs.set(i,j, counts.get(i,j)/counts_sum);
            }
        }

        RowColIndex rci = BallPathGenerator::Choice(probs);
        counts.decreaseBy1(rci.r,rci.c);

        outputs << rci;


    }

    return outputs;

}

BallPathGenerator::RowColIndex BallPathGenerator::Choice1D(const SimpleMatrix &probs){

    QList<qreal> probs_acc;
    SimpleMatrix::MatDim md = probs.getDim();
    qsizetype N = md.ncols;
    probs_acc.fill(0,N);

    probs_acc[0] = probs.get(0,0);
    for (qsizetype i = 1; i < N; i++){
        probs_acc[i] = probs_acc[i-1] + probs.get(0,i);
    }

    // Random value between 0 and 1 (1, not includede).
    qreal random_value = QRandomGenerator::global()->generateDouble();

    qint32 index = 0;
    while (probs_acc[index] < random_value){
        index++;
        if (index == (N-1)) break; // In this case the index is the last one.
    }

    RowColIndex rci;
    rci.i = index;
    return rci;

}

BallPathGenerator::RowColIndex BallPathGenerator::Choice(const SimpleMatrix &probs){

    qint32 cols = probs.getDim().ncols;
    RowColIndex rci = BallPathGenerator::Choice1D(probs.flatten());
    qint32 index = rci.i;
    rci.r  = index/cols;
    rci.c = index - rci.r*cols;
    return rci;

}

qreal BallPathGenerator::MeanOnList(const QList<qreal> &list){

    if (list.size() == 0) return 0;

    qreal N = list.size();
    qreal a = 0;
    for (qint32 i = 0; i < list.size(); i++){
        a = a + list.at(i);
    }

    return a/N;

}

