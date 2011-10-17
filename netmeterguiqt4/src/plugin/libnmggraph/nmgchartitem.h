/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef NMGCHARTITEM_H
#define NMGCHARTITEM_H

#include "nmgglobals.h"
#include <QGraphicsItem>
#include <QPainter>
#include <QObject>


/** \brief NMGChartItem is the base class for all grafics items in the NMGChartScene. */
class NMGChartItem : public QObject, public QGraphicsItem
{
  Q_OBJECT
  
  public:
    
    /** \brief Type of chart items supported. */
    enum ChartItemType
    {
      ROOT,
      LABEL,
      WALL,
      SERIES,
      AXIS,
      LEGEND
    };
    
    /** \brief Default constructor. */
    NMGChartItem(QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartItem() {}
    
    /** \brief Returns the type of chart item. Method provided for convenience. */
    virtual ChartItemType getTypeOfChartItem() const = 0;
    
    /** 
     * \brief Saves the current width in px that item takes up into the scene.
     * \param width Current item width in px.
     */
    void setCurrentWidthIntoScene(const int& width);
    
    /** 
     * \brief Returns the current width in px that item takes up into the scene.
     * \return Current item width in px.
     */
    inline const int& getCurrentWidthIntoScene() const { return currentWidthIntoScene; }
    
    /** 
     * \brief Saves the current height in px that item takes up into the scene.
     * \param height Current item height in px.
     */
    void setCurrentHeightIntoScene(const int& height);
    
    /** 
     * \brief Returns the current height in px that item takes up into the scene.
     * \return Current item height in px.
     */
    inline const int& getCurrentHeightIntoScene() const { return currentHeightIntoScene; }
    
    /** 
     * \brief Method provided for convinience. Mixes setCurrentWidthIntoScene() and 
     * setCurrentHeightIntoScene() methods behaviour into one.
     * \param width Current item width in px.
     * \param height Current item height in px.
     */
    void setCurrentWidthHeightIntoScene(const int& width, const int& height);
    
    /** \brief Returns the minimum width needed by the item to be shown correctly. */
    virtual inline int getSuggestedMinimumWidthIntoScene() const { return 0; }
    
    /** \brief Returns the minimum height needed by the item to be shown correctly. */
    virtual inline int getSuggestedMinimumHeightIntoScene() const { return 0; }
    
    /** 
     * \brief Returns the current item padding margin in px.
     * \return Current item padding margin in px.
     */
    inline const int getCurrentPaddingMargin() const { return currentPaddingMargin; }
    
  signals:
    
    /** 
     * \brief This signal is emitted when the space needed by the item to be painted 
     * correctly is less than the space gave by the NMGChartScene. 
     * 
     * Needed space can be obtained with getSuggestedMinimumHeightIntoScene() and 
     * getSuggestedMinimumWidthIntoScene().
     */
    void signalGiveMeMoreSpace();
  
  protected:

    /** 
     * \brief Specifies the current width in px that item takes up into the scene. 
     * 
     * By default is set to 0.
     */
    int currentWidthIntoScene;
    
    /** 
     * \brief Specifies the current height in px that item takes up into the scene. 
     * 
     * By default is set to 0.
     */
    int currentHeightIntoScene;
    
    /** \brief Specifies the current padding margin in px inside the item. 
     * 
     * By default is set to 0. Any derived class can give another value to the default one
     */
    int currentPaddingMargin;
    
    /** 
     * \brief Records if the space available into scene for the item has changed due 
     * to some resize into scene. 
     * 
     * It is updated by setCurrentWidthIntoScene(), setCurrentHeightIntoScene() and 
     * setCurrentWidthHeightIntoScene() methods. Later use and update can be done in 
     * derived classes. By default is set to false;
     */
    bool spaceAvailableIntoSceneChanged;
        
    /** 
     * \brief Records that the space needed to paint correctly an item has changed.
     * 
     * It is used by derived classes that receive external changes to record that the item 
     * state has changed in such a way that its size has been modified and its painting  may 
     * be affected by this change. By default is set to false.
     * 
     * \sa hasEnoughSpace()
     */
    bool spaceNeededIntoSceneChanged;
    
    /**
     * \brief Determines if the current available space into scene is enough to paint 
     * correctly the item according to its needed minimum space.
     * 
     * It is used by derived classes to determine if it is necessary to notify the scene that 
     * it has to increase the space gave to the item.
     * 
     * \sa signalGiveMeMoreSpace
     */
    bool hasEnoughSpace();
    
    /** 
     * \brief Specifies the height in px that item needs to be painted correctly. 
     * 
     * By default is set to 0. Used internally for perfomance issues. It shouldn't be used
     * directly in derived classes.
     */
    int neededMinimumHeightIntoScene;
    
    /** 
     * \brief Specifies the width in px that item needs to be painted correctly. 
     * 
     * By default is set to 0. Used internally for perfomance issues. It shouldn't be used
     * directly in derived classes.
     */
    int neededMinimumWidthIntoScene;
    
};

/** 
 * \brief NMGChartRoot class acts as a container for all chart items. 
 * 
 * It is a convinience class that allows to define a hierarchy between items.
 */
class NMGChartRoot : public NMGChartItem
{
  public:
    
    /**
     * \brief Default constructor.
     * \param parent NMGChartItem parent, if any. Because this is the first element in the
     * hierarchy, parent shouldn't be specified.
     */ 
    NMGChartRoot(QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartRoot() {}
  
    /** \brief Defines the outer bounds of the item as a rectangle. */
    QRectF boundingRect() const;
    
    /** \brief Provides the item painting implementation. */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
    inline ChartItemType getTypeOfChartItem() const { return NMGChartItem::ROOT; }
};

/**
 * \brief NMGChartLabel class generates graphical labels.
 * 
 * It includes three types of graphical labels: title, x axis and y axis.
 */
class NMGChartLabel : public NMGChartItem
{
  Q_OBJECT
  Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
  Q_PROPERTY(QString text READ getText WRITE setText)
  Q_PROPERTY(QColor color READ getBaseColor WRITE setBaseColor)
  Q_PROPERTY(QString fontFamily READ getFontFamily WRITE setFontFamily)
  Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize)
  
  public:
    
    /** \brief Supported labels. */
    enum LabelType
    {
      TITLE,
      AXIS_X,
      AXIS_Y
    };
    
    /** 
     * \brief Default constructor.
     * \param type Type of label.
     * \param parent NMGChartItem parent, if any.
     */
    NMGChartLabel(NMGChartLabel::LabelType type, QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartLabel() {}

    /** \brief Defines the outer bounds of the item as a rectangle. */
    QRectF boundingRect() const;
    
    /** \brief Provides the item painting implementation. */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
    inline ChartItemType getTypeOfChartItem() const { return NMGChartItem::LABEL; }
    
    /** \brief Returns the type of label. */
    inline LabelType getTypeOfLabel() const { return type; }
    
    int getSuggestedMinimumWidthIntoScene() const;
    
    int getSuggestedMinimumHeightIntoScene() const;    
    
    /** 
     * \brief Changes the text that is shown.
     * \param text New text.
     */
    void setText(QString text);
    
    /**
     * \brief Sets the label content color.
     * \param color Label color.
     */
    void setBaseColor(QColor color);
    
    /**
     * \brief Sets the label font family.
     * \param family Font family.
     */
    void setFontFamily(QString family);
    
    /**
     * \brief Sets the label font size.
     * \param size Font size.
     */
    void setFontSize(int size);
    
    /** \brief Returns the current label text. */
    inline const QString& getText() const { return currentText; }
    
    /** \brief Returns the label color. */
    inline const QColor& getBaseColor() const { return baseColor; }
    
    /** \brief Returns the label font family. */
    inline const QString getFontFamily() const { return currentFont.family(); }
    
    /** \brief Returns the label font size. */
    inline const int getFontSize() const { return currentFont.pointSize(); }

  private:
    
    LabelType type;
    QFont currentFont;
    QString currentText;
    QColor baseColor;
};

/** \brief NMGChartWall acts as a container element for NMGChartSeries. */
class NMGChartWall : public NMGChartItem
{
  Q_OBJECT
  Q_PROPERTY(bool showXReferenceLines READ areXReferenceLinesVisible 
                                      WRITE setXReferenceLinesVisibility)
  Q_PROPERTY(bool showYReferenceLines READ areYReferenceLinesVisible 
                                      WRITE setYReferenceLinesVisibility)
  public:
    
    /** 
     * \brief Default constructor.
     * \param parent NMGChartItem parent, if any.
     */
    NMGChartWall(QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartWall() {}

    /** \brief Defines the outer bounds of the item as a rectangle. */
    QRectF boundingRect() const;
    
    /** \brief Provides the item painting implementation. */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
    inline ChartItemType getTypeOfChartItem() const { return NMGChartItem::WALL; }
    
    /** \brief Returns a new color in a defined scale of color. */
    QColor getNextSeriesBaseColor();
    
    /** 
     * \brief Sets the grid values for the reference lines.
     * \param xAxis Number of reference lines in x-axis. The minimum is 2.
     * \param yAxis Number of reference lines in y-axis. The minimum is 2.
     */
    void setNumReferenceLines(int xAxis = 2, int yAxis = 2);
    
    /** \brief Returns true if vertical reference lines are visible; False otherwise. */
    inline const bool areXReferenceLinesVisible() const { return areXLinesVisible; }
    
    /** \brief Returns true if horizontal reference lines are visible; False otherwise. */
    inline const bool areYReferenceLinesVisible() const { return areYLinesVisible; }
    
    /**
     * \brief Sets the visibility for the vertical reference lines.
     * \param areVisible Visibility.
     */
    void setXReferenceLinesVisibility(bool areVisible);

    /**
     * \brief Sets the visibility for the horizontal reference lines.
     * \param areVisible Visibility.
     */
    void setYReferenceLinesVisibility(bool areVisible);
    
    /**
     * \brief This method is used to notify chart wall, which is the parent for all series,
     * that an specific child series has been deleted.
     * 
     * This method not only updates chart wall's children but also deletes the child itself. 
     * In that sense, this method acts as the default destructor but for a single child.
     */
    void deleteChildSeries(QGraphicsItem* child);
    
  private:
        
    int lastSeriesColor;
    int numXReferenceLines; /**< Number of dividing reference lines in X axis. */
    int numYReferenceLines; /**< Number of dividing reference lines in Y axis. */
    bool areXLinesVisible;
    bool areYLinesVisible;
};

/** 
 * \brief NMGChartSeries is an abstraction of a data series.
 * 
 * It is one of the most complex classes in the chart item collection because it has been
 * designed to supported a huge quantity of data in each series and manage it as fast as 
 * possible. Series data is composed of a collection of 2D points that are rendered in its 
 * available 2D space.
 */
class NMGChartSeries : public NMGChartItem
{
  Q_OBJECT
  Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
  Q_PROPERTY(QString text READ getText WRITE setText)
  Q_PROPERTY(QColor color READ getBaseColor WRITE setBaseColor)  
  Q_PROPERTY(bool average 
             READ getAverageState 
             WRITE setAverageState 
             DESIGNABLE isAveragedStateDesignable)
  Q_PROPERTY(QString representation READ getSeriesType WRITE setSeriesType)
  Q_PROPERTY(int pointSize 
             READ getPointSize 
             WRITE setPointSize
             DESIGNABLE isPointSizeDesignable)
  
  public:
        
    /** 
     * \brief Default constructor.
     * \param id Internal identifier
     * \param parent NMGChartItem parent, if any. I should be NMGChartWall
     */
    NMGChartSeries(QString id, QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartSeries() {}

    /** \brief Defines the outer bounds of the item as a rectangle. */
    QRectF boundingRect() const;
    
    /** \brief Provides the item painting implementation. */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option = 0, QWidget* widget = 0);
    
    inline ChartItemType getTypeOfChartItem() const { return NMGChartItem::SERIES; }
    
    /** \brief Returns the internal series identifier. */
    inline const QString& getId() const { return id; }
    
    /**
     * \brief Sets the series visibility.
     * \param visible Visibility.
     */
    void setVisible(bool visible);
    
    /** 
     * \brief Sets a visible text for the series. Is is normally used in a legend.
     * \param text Visible text associated to the series.
     */
    void setText(QString text);
    
    /**
     * \brief Sets the color the series will used to be painted.
     * \param color Series color.
     */
    void setBaseColor(QColor color);
    
    /**
     * \brief Sets the average state in the series. 
     * This is used to simplify the number of data being painted. When two or more different 
     * points in the series are mapped into the same vertical pixel into the viewport, they 
     * are all averaged according to its y-coordinate into a new point, that represents all 
     * them.
     * It is important to know that setAverageState and setAccumulateState are excluding 
     * methods, because only one must be set to true. If \e average is set to true, 
     * accumulate state is automatically set to false, and viceversa.
     * \param averaged True if average has to be activated; false otherwise
     */
    void setAverageState(bool averaged);
    
    /**
     * \brief Sets the accumulate state in the series. 
     * 
     * This is used to simplify the number of data being painted. When two or more different
     * points in the series are mapped into the same vertical pixel into the viewport, they 
     * are all added (or accumulate) according to its y-coordinate into a new point, that 
     * represents all them.
     * It is important to know that setAccumulateState and setAverageState are excluding 
     * methods, because only one must be set to true. If \e accumulate is set to true, 
     * average state is automatically set to false, and viceversa.
     * \param accumulated True if accumulate has to be activated; false otherwise. 
     */
    void setAccumulateState(bool accumulated);
    
    /** 
     * \brief Sets the clipping state for the series.
     * 
     * Only use when clipping is really needed because it adds an additional computational overhead.
     * \param clipped When true all series data painted outside its assigned scene space will be clipped.
     */
    void setClippingState(bool clipped);
    
    /**
     * \brief Sets the representation used to paint the series. It must be LINE, AREA,
     * POINTS or BARS. If \e type isn't any of these, the method has not effect.
     * \param type Series representation.
     */
    void setSeriesType(QString type);
    
    /**
     * \brief Sets the point size used to paint the series in POINTS mode.
     * \param size Size must be a positive value. Otherwise this method has not effect
     */
    void setPointSize(int size);
    
    /** \brief Returns the visible text associated to the series. */
    inline const QString& getText() const { return currentText; }
    
    /** \brief Returns the series color. */
    inline const QColor& getBaseColor() const { return baseColor; }
    
    /** \brief Returns the series average state. */
    inline bool getAverageState() const { return isAveraged; }
    
    /** \brief Returns the series accumulate state. */
    inline bool getAccumulateState() const { return isAccumulated; }
    
    /** 
     * \brief Returns the representation used to paint the series. It can be LINE, AREA or
     * POINTS.
     */
    inline const QString& getSeriesType() const { return representation; }
    
    /** \brief Returns the point size used to paint the series in POINTS mode. */
    inline const int getPointSize() const { return pointSize; }
    
    /** 
     * \brief Loads a collection of 2D points into the series.
     * 
     * Points will only be loaded if \e xData and \e yData are not empty and contains the
     * same number of elements.
     * 
     * \param xData horizontal 2D point values.
     * \param yData vertical 2D point values.
     */ 
    void setVertexList(const QList<double>& xData, const QList<double>& yData);
    
    /** 
     * \brief Returns the current maximum horizontal value in the series set of points.
     * 
     * It is very unlikely that you will ever need this function. 
     */
    inline const double& getMaxXValue() const { return maxValues.x; }
    
    /** 
     * \brief Returns the current maximum vertical value in the series set of points.
     * 
     * It is very unlikely that you will ever need this function. 
     */
    inline const double& getMaxYValue() const { return maxValues.y; }
    
    /** 
     * \brief Returns the current minimum horizontal value in the series set of points.
     * 
     * It is very unlikely that you will ever need this function. 
     */
    inline const double& getMinXValue() const { return minValues.x; }
    
    /** 
     * \brief Returns the current minimum vertical value in the series set of points.
     * 
     * It is very unlikely that you will ever need this function. 
     */
    inline const double& getMinYValue() const { return minValues.y; }
    
    /**
     * \brief Sets the maximum values for a point in the series. 
     * 
     * It doesn't add any extra point or modify any existing one. It only saves a maximum 
     * reference point. It is very unlikely that you will ever need this function. Use only 
     * if you know how to use it, otherwise you can obtain an unpredictable and unsafe 
     * behaviour. The effect of this method is the redistribution of series points in its
     * 2D space when they are rendered.
     */
    void setMaxValues(const double& x, const double& y);
    
    /**
     * \brief Sets the minimum values for a point in the series. 
     * 
     * It doesn't add any extra point or modify any existing one. It only saves a maximum 
     * reference point. It is very unlikely that you will ever need this function. Use only 
     * if you know how to use it, otherwise you can obtain an unpredictable and unsafe 
     * behaviour. The effect of this method is the redistribution of series points in its
     * 2D space when they are rendered.
     */
    void setMinValues(const double& x, const double& y);
    
    /** \brief Returns the original maximum horizontal value in the series set of points. */
    inline const double& getOriginalMaxXValue() const { return origMaxValues.x; }
    
    /** \brief Returns the original maximum vertical value in the series set of points. */
    inline const double& getOriginalMaxYValue() const { return origMaxValues.y; }
    
    /** \brief Returns the original minimum horizontal value in the series set of points. */
    inline const double& getOriginalMinXValue() const { return origMinValues.x; }
    
    /** \brief Returns the original minimum vertical value in the series set of points. */
    inline const double& getOriginalMinYValue() const { return origMinValues.y; }
    
  signals:
    
    /** \brief This signal is emitted to notify that text in the series has been changed. */
    void signalTextChanged(const QString& id, const QString& text);
    
    /** \brief This signal is emitted to notify that color in the series has been changed. */
    void signalColorChanged(const QString& id, const QColor& color);
    
    /** \brief This signal is emitted to notify that series visibility has been changed. */
    void signalVisibilityChanged(const QString& id, bool visible);
            
  private:
    
    inline double xWindowToViewport(const double& xcoord);
    inline double yWindowToViewport(const double& ycoord);
    void calculateScaleFactors();
    void paintLine(QPainter* painter, 
                   const QStyleOptionGraphicsItem* option = 0, 
                   QWidget* widget = 0);
    void paintArea(QPainter* painter, 
                   const QStyleOptionGraphicsItem* option = 0, 
                   QWidget* widget = 0);
    void paintPoints(QPainter* painter, 
                     const QStyleOptionGraphicsItem* option = 0, 
                     QWidget* widget = 0);
    void paintBars(QPainter* painter, 
                   const QStyleOptionGraphicsItem* option = 0, 
                   QWidget* widget = 0);
    /** 
     * \brief Determines if average property is suitable for modification by the
     * property inspector.
     */
    inline bool isAveragedStateDesignable() const { return !isAccumulated; }
    
    /**
     * \brief Determines if point size property is suitable for modification by the
     * property inspector.
     */
    inline bool isPointSizeDesignable() const { return representation == POINTS_TYPE; }
    
    QString id; /**< Internal identifier.*/
    QString currentText; /**< Visible external text associated to the series.*/
    QColor baseColor; /**< Series color.*/
    bool isAveraged; /**< Determines if points in the series have to be averaged.*/
    bool isAccumulated; /**< Determines if points in the series have to be accumulated.*/
    bool isClipped; /**< Determines if series is clipped when painted outside its space. */
    QString representation; /**< Saves the type of representation (line, points, etc).*/
    int pointSize; /**<Saves the point size used to paint in POINTS mode.*/

    // Definition of a dot in 2D
    struct Vertex{
      double x;
      double y;
      
      Vertex() {}
      Vertex (const double& x, const double& y)
        : x(x), y(y)
      {}
    };

    QList<Vertex> vertexList; /**< Series data. */

    Vertex minValues; /**< Bottom left vertex (real or imaginary). */
    Vertex maxValues; /**< Top right vertex (real or imaginary). */
    
    Vertex origMinValues; /** Original bottom left vertex. */
    Vertex origMaxValues; /** Original top right vertex. */
    
    double verticalScaleFactor; /**< Used for performance issues. */
    double horizontalScaleFactor; /**< Used for performance issues. */
    bool rebuildScaleFactors; /**< Determines if scale factors need to be rebuild. */
};

/**
 * \brief NMGChartAxis class is a base class that defines common methods for the two types
 * of axes (x-axis and y-axis).
 * 
 * This class has been designed to support number and text based axes. Number based axes
 * result in a set of numeric labels that defines the scale of values. They are rounded
 * according to an algorithm that only takes the most significat digits without loss of
 * accurancy.<br>
 * Text based axes have more freedom because is the user who establish the set of textual
 * labels that defines the categorical scale.
 */
class NMGChartAxis : public NMGChartItem
{
  Q_OBJECT
  Q_PROPERTY(QString fontFamily READ getFontFamily WRITE setFontFamily)  
  Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize)

  public:
    
    /** Supported type of labels. */
    enum DataType
    {
      NUMBER,
      TEXT
    };

    /** 
     * \brief Default constructor.
     * \param parent NMGChartItem parent, if any.
     */
    NMGChartAxis(QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartAxis() {}
    
    inline ChartItemType getTypeOfChartItem() const { return NMGChartItem::AXIS; }

    /** 
     * \brief Changes the type of labels that are managed by the axis. 
     * 
     * When an axis is created its type is NUMBER. This method resets the axis labels to 
     * its default ones according to the type assigned. If \e newType is NUMBER two labels 
     * are built, 0 and 1. Otherwise, if \e newType is TEXT any label is built.
     * \param newType Label type.
     */
    void setDataType(const NMGChartAxis::DataType newType);
    
    /** \brief Returns the type of labels managed by the axis. */
    inline NMGChartAxis::DataType getDataType() const { return type; }
    
    /** 
     * \brief Defines the number of labels that appear in the axis. 
     * 
     * This method only has effect in numeric type labels.
     * \param value Number of labels. It must be greater or equal to 2 because in a numeric
     * axis minimum and maximum values are needed to define correctly the scale values.
     */
    void setNumReferenceValues(const int value = 2);
    
    /** \brief Returns the number of labels that appear in the axis. */
    inline int getNumReferenceValues() { return numReferenceValues; }
    
    /**
     * \brief Defines the minimum and maximum values in the scale.
     * 
     * This method only has effect in numeric type labels.
     * \param minValue Scale minimum value.
     * \param maxValue Scale maximum value.
     */
    void setAxisScale(const double minValue, const double maxValue);
    
    /**
     * \brief Adds a textual label to the set of axis labels.
     * 
     * This method only has effect in textual type labels.
     * \param id Any internal identifier. It is used to change or order labels in a later moment.
     * Must be different for each label.
     * \param text Label text.
     */
    void addLabel(const QString& id, QString text);
    
    /**
     * \brief Removes a textual label previously added.
     * 
     * This method only has effect in textual type labels.
     * \param id Any internal identifier. If the identifier doesn't exist this method has not
     * effect.
     */
    void removeLabel(const QString& id);
    
  public slots:
    
    /**
     * \brief Update the visual text associated to the label identified by \e id . 
     * 
     * It has not effect if it doesn't exist any label identified by \e id. 
     * \param id Internal identifier added previously for the label to change.
     * \param newText New label text.
     */
    void slotChangeLabelText(const QString& id, const QString& newText);
    
    /**
     * \brief Change the order of the label identified by \e id inside the labels collection. 
     * 
     * It has not effect if it doesn't exist any label identified by \e id or the order is not
     * valid. 
     * \param id Internal identifier added previously for the label to change.
     * \param newOrder New order position. Must be between 0 and the maximum number of labels
     * minus one.
     */
    void slotChangeLabelOrder(const QString& id, int newOrder);
    
  protected:
    
    /**
     * \brief Sets the axis font family.
     * \param family Font family.
     */
    void setFontFamily(QString family);
    
    /**
     * \brief Sets the axis font size.
     * \param size Font size.
     */
    void setFontSize(int size);
    
    /** \brief Returns the axis font family. */
    inline const QString getFontFamily() const { return currentFont.family(); }
    
    /** \brief Returns the axis font size. */
    inline const int getFontSize() const { return currentFont.pointSize(); }
    
    /** 
     * \brief Location of axis labels. 
     * 
     * It determines if labels are only painted inside its item space or take some extra space.
     */
    enum LabelsLocation
    {
      INTERNAL,  /**< This location is only used in TEXT type labels. */
      EXTERNAL  /**< This location is only used in NUMBER type labels. */
    };
    
    LabelsLocation location; /**< Contains the current labels location. */
    DataType type; /**< Contains the data type used in labels. */
    QFont currentFont; /**< Font used to print the axis labels. */
    int numReferenceValues; /**< Number of values in which axis is divided. */
    QStringList labelsList; /**< Formatted labels. */
    QStringList idsList; /**< Internal id associated with each label. Only used in TEXT mode*/
    
  private:
    
    void recalculateAxisLabels();
    
    double minLabelValue; /**< Minimum axis value when it is numeric. */
    double maxLabelValue; /**< Maximum axis value when it is numeric. */ 
};

/**
 * \brief NMGChartXAxis is the horizontal version of NMGChartAxis.
 * 
 * Its labels are rotated 90º degrees contrary to the clockwise before being rendered
 * and distributed along the available item space according to its location (external or
 * internal).
 */
class NMGChartXAxis : public NMGChartAxis
{
  public:
    
    /** 
     * \brief Default constructor.
     * \param parent NMGChartItem parent, if any.
     */
    NMGChartXAxis(QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartXAxis() {}
    
    /** 
     * \brief Defines the outer bounds of the item as a rectangle. 
     * 
     * Outer bounds differ from external to internal location. The first ones will be slighty
     * wider than the second ones.
     */    
    QRectF boundingRect() const;
    
    /** \brief Provides the item painting implementation. */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
    int getSuggestedMinimumWidthIntoScene() const;
    int getSuggestedMinimumHeightIntoScene() const;

};

/**
 * \brief NMGChartYAxis is the vertical version of NMGChartAxis.
 * 
 * Its labels are rendered horizontally and distributed along the available item space 
 * according to its location (external or internal).
 */
class NMGChartYAxis : public NMGChartAxis
{
  public:
        
    /** 
     * \brief Default constructor.
     * \param parent NMGChartItem parent, if any.
     */
    NMGChartYAxis(QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartYAxis() {}
    
    /** 
     * \brief Defines the outer bounds of the item as a rectangle. 
     * 
     * Outer bounds differ from external to internal location. The first ones will be slighty
     * higher than the second ones.
     */    
    QRectF boundingRect() const;
    
    /** \brief Provides the item painting implementation. */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
    int getSuggestedMinimumWidthIntoScene() const;
    int getSuggestedMinimumHeightIntoScene() const;

};

/** \brief NMGChartLegend class generates a graphical brief description identifying series. */
class NMGChartLegend : public NMGChartItem
{
  Q_OBJECT
  Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
  Q_PROPERTY(QString fontFamily READ getFontFamily WRITE setFontFamily)
  Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize)
  Q_PROPERTY(QString position READ getPosition WRITE setPosition)
  
  public:
    
    /** 
     * \brief Default constructor.
     * \param parent NMGChartItem parent, if any.
     */
    NMGChartLegend(QGraphicsItem* parent = 0);
    
    /** \brief Default destructor. */
    virtual ~NMGChartLegend() {}

    /** \brief Defines the outer bounds of the item as a rectangle. */
    QRectF boundingRect() const;
    
    /** \brief Provides the item painting implementation. */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
    inline ChartItemType getTypeOfChartItem() const { return NMGChartItem::LEGEND; }
    
    int getSuggestedMinimumWidthIntoScene() const;
    
    int getSuggestedMinimumHeightIntoScene() const;
    
    /**
     * \brief Sets the label font family.
     * \param family Font family.
     */
    void setFontFamily(QString family);
    
    /**
     * \brief Sets the label font size.
     * \param size Font size.
     */
    void setFontSize(int size);
    
    /**
     * \brief Sets the position where the legend will be located. It must be "North",
     * "Northeast", "East", "Southeast", "South", "Southwest", "West", "Northwest" or
     * "Center". If \e position isn't any of these, the method has not effect.
     * \param position Legend location.
     */
    void setPosition(QString position);
    
    /** \brief Returns the label font family. */
    inline const QString getFontFamily() const { return currentFont.family(); }
    
    /** \brief Returns the label font size. */
    inline const int getFontSize() const { return currentFont.pointSize(); }
    
    /** 
     * \brief Returns the legend location. It can be "North", "Northeast", "East", 
     * "Southeast", "South", "Southwest", "West", "Northwest" or "Center".
     */
    inline const QString getPosition() { return positionMapping.value(currentPosition); }
    
    
  public slots:
    
    /** 
     * \brief This slot should be called each time a new series is added to the scene.
     * 
     * It allows to update information in legend accordingly, adding a new entry for the
     * new series. By default the legend entry visibility is enabled.
     * \param seriesId Internal series identifier.
     * \param text Legend entry text.
     * \param color Legend entry decoration color.
     */
    void slotAddNewEntry(const QString& seriesId, const QString& text, const QColor& color);
    
    /**
     * \brief This slot should be called each time a series is removed from the scene.
     * \param seriesId Internal series identifier.
     */
    void slotDeleteEntry(const QString& seriesId);
    
    /**
     * \brief Updates the legend entry text identified with \e seriesId.
     * 
     * If the entry is not found or the text is the same, the slot has not effect.
     * \param seriesId Internal series identifier.
     * \param text New text.
     */
    void slotChangeEntryText(const QString& seriesId, const QString& text);
    
    /**
     * \brief Updates the legend entry decoration identified with \e seriesId.
     * 
     * If the entry is not found or the decoration color is the same, the slot 
     * has not effect.
     * \param seriesId Internal series identifier.
     * \param color New color.
     */
    void slotChangeEntryColor(const QString& seriesId, const QColor& color);
    
    /**
     * \brief Reorders a legend entry to a new order position.
     * 
     * This method assumes the existence of the entry identified with \e seriesId and
     * a valid order position. Otherwise it has not effect.
     * \param seriesId Internal series identifier.
     * \param newOrder New order position. Must be between 0 and the maximum number of entries
     * minus one.
     */
    void slotChangeEntryOrder(const QString& seriesId, int newOrder);
    
    /**
     * \brief Sets the visibility for a legend entry identified with \e seriesId .
     * 
     * If the entry is not found, the slot has not effect.
     * \param seriesId Internal series identifier.
     * \param visible Visibility.
     */
    void slotChangeEntryVisibility(const QString& seriesId, bool visible);

  private:
    
    void translateToCurrentPosition(QPainter* painter);
    int numberOfVisibleEntries() const;
    
    enum LegendPosition { N, NE, E, SE, S, SW, W, NW, C };
    LegendPosition currentPosition;
    QFont currentFont;
    
    // Definition of a legend entry
    struct LegendEntry{
      QColor color;
      QString text;
      bool visible;
      
      LegendEntry() {}
      LegendEntry(const QColor& color, const QString& text, bool visible)
        : color(color), text(text), visible(visible)
      {}
    };

    QMap<LegendPosition, QString> positionMapping; /**< Maps position abbreviation with its text.*/
    QMap<QString, LegendEntry> legendEntries; /**< Saves the individual legend entry data.*/
    QList<QString> entriesOrder; /**< Mantains the order in which entries appears in legend.*/
    
    /** Saves the number of pixels taken into account to construct the legend entry decoration.*/
    int decorationPixels; 
    /** Saves the witdh between decoration and text en each entry (in pixels). */ 
    int decorationToTextPadding;
    int internalPaddingMargin;
    QColor background;
    
};

#endif
