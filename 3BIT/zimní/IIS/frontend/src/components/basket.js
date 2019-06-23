import React, {Component} from 'react';
import {Link, Redirect} from "react-router-dom";
import axios from 'axios';
//
import {API_BASE, ROUTE_PREFIX, getUser, on403, getBasketId, BASKET_NAME} from '../const.js'

class Basket extends Component {

    constructor(props) {
        super(props)
        this.state = {
            data: '',
            redirect: false
        }
    }


    async componentDidMount() {
        if (JSON.parse(getBasketId())['basketId'] && JSON.parse(getBasketId())['email'] === getUser()['email']) {
            try {
                // najdeme data na upravu
                var response = await axios({
                    method: 'get',
                    url: `${API_BASE}order/${JSON.parse(getBasketId())['basketId']}`,
                    headers: {
                        token: getUser()['token']
                    }
                })
                this.setState({data: response.data})
            } catch (error) {
                if (error.response.status === 403) {
                    on403()
                }
            }
        }
    }

    handleButton = async (type, id, event) => {
        if (type === 'odebrat') {
            try {
                await axios({
                    method: 'delete',
                    url: `${API_BASE}order/${JSON.parse(getBasketId())['basketId']}`,
                    headers: {
                        token: getUser()['token']
                    },
                    data: {
                        varka_id: id
                    }
                })
                if (!(this.state.data.varka.length - 1)) {
                     this.handleButton('odebratObjednavku', '', event)
                } else {
                    window.location.reload()
                }
            } catch (error) {
                on403()
            }
        } else if (type === 'odebratObjednavku') {
            try {
                await axios({
                    method: 'delete',
                    url: `${API_BASE}order/${JSON.parse(getBasketId())['basketId']}`,
                    headers: {
                        token: getUser()['token']
                    },
                    data: {
                    }
                })

                localStorage.removeItem(BASKET_NAME)
                window.location.reload()
            } catch (error) {
                on403()
            }
        } else {
            try {
                var response = await axios({
                    method: 'put',
                    url: `${API_BASE}order/${JSON.parse(getBasketId())['basketId']}`,
                    headers: {
                        token: getUser()['token']
                    },
                    data: {
                        stav: 1
                    }
                })

                //console.log(response)
                localStorage.removeItem(BASKET_NAME)
                alert('Děkujeme za vaši objednávku')
                this.setState({redirect: true})
            } catch (error) {
                on403()
            }
        }
    }

    render() {

        if (this.state.redirect) {
            return <Redirect to={`${ROUTE_PREFIX}`}/>
        }
        //console.log(this.state.data)

        var content

        if (this.state.data) {
            var sumPrice = 0

            return (
                <div>
                    <nav className="subHeader__nav--small">
                        <div className="layout__inner">
                            <Link to={`${ROUTE_PREFIX}`}>Zpátky do obchodu</Link>
                        </div>
                    </nav>
                    

                     <div className="admin admin__start layout__inner">
                        <div className="admin__start__text">
                            <h1>Váš košík</h1>
                        </div>


                        <table className="admin__table" cellSpacing="0" cellPadding="0">
                        <thead>
                            <tr>
                                <th>Jméno čaje</th>
                                <th>Várka</th>
                                <th>Množství (g)</th>
                                <th>Cena (kč)</th>
                                <th></th>
                            </tr>
                        </thead>
                        <tbody>
                            {this.state.data.varka.map((item) => {
                                sumPrice += item.cena / 5 * item.mnozstvi
                                return (
                                    <tr key={item.varka_id}>
                                        <td>{item.jmeno_caje}</td>
                                        <td>{item.varka_id}</td>
                                        <td>{item.mnozstvi}</td>
                                        <td>{item.cena / 5 * item.mnozstvi}</td>
                                        <td>
                                            <button className="button__secondary" onClick={(event) => this.handleButton('odebrat', item.varka_id, event)}>Odebrat</button>
                                        </td>
                                    </tr>
                                ) 
                            })}
                        </tbody>
                        </table>
                        <div>
                            <h2 style={{textAlign: 'center', marginBottom: '20px'}}>Celkem: {sumPrice} kč</h2>
                            <button className="button" onClick={(event) => this.handleButton('potvrdit', '', event)}>Potvrdit objednávku</button>
                            <button className="button__secondary" onClick={(event) => this.handleButton('odebratObjednavku', '', event)}>Zrušit objednávku</button>
                        </div>
                    </div>
                </div>
            )


        } else {
            return (
                <div>
                    <nav className="subHeader__nav--small">
                        <div className="layout__inner">
                            <Link to={`${ROUTE_PREFIX}`}>Zpátky do obchodu</Link>
                        </div>
                    </nav>
                    
                     <div>
                        <div className="admin admin__start layout__inner">
                            <h1>Prázdné</h1>
                        </div>
                    </div>
                </div>
            )
        }
    }
    
}

export default Basket;